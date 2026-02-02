# Corrections des problèmes identifiés dans la simulation

## Résumé des problèmes

| Problème | Fichier concerné | Statut |
|----------|------------------|--------|
| Overlap PVC Wall/Bottom (~496 µm) | `DetectorConstruction.cc` | ✅ Corrigé |
| WaterRings_passages: 0 entrées | `ScorePlane4SD.cc` | ✅ Corrigé |

---

## Problème 1 : Overlap géométrique PVC

### Diagnostic

Le log Geant4 affichait :
```
Overlap is detected for volume physPVCWall:0 (G4Tubs) with physPVCBottom:0 (G4Tubs)
overlap at local point (2.11321,9.77417,0.00443169) by 495.568 um
```

### Cause

La paroi PVC et le fond PVC se chevauchaient dans la région z ∈ [68, 69] mm.

**Configuration incorrecte :**
```cpp
containerWallHeight = 4 mm
zPVCWallCenter = 67 mm
→ Paroi de z = 65 mm à z = 69 mm

zPVCBottomCenter = 68.5 mm
→ Fond de z = 68 mm à z = 69 mm

OVERLAP : z = 68 à 69 mm (~1 mm de chevauchement)
```

### Solution

Dans `DetectorConstruction.cc`, remplacer les lignes 328-336 :

**AVANT (incorrect) :**
```cpp
const G4double containerHeight = 5.0*mm;
const G4double containerWallHeight = containerHeight - pvcThickness; // = 4 mm
// ...
const G4double zPVCWallCenter = zWaterFrontSurface + containerWallHeight/2.0; // 67 mm
```

**APRÈS (corrigé) :**
```cpp
const G4double containerWallHeight = waterThickness;  // = 3 mm
// ...
const G4double zPVCWallCenter = zWaterCenter;  // = 66.5 mm
```

### Géométrie corrigée

```
                     z (mm)
                       |
    ┌─────────────────┐ 69.0 ─── Fond PVC (surface arrière)
    │▓▓▓ PVC Fond ▓▓▓│ 68.5 ─── Centre fond PVC
    ├─────────────────┤ 68.0 ─── Interface eau/fond (PAS D'OVERLAP)
    │                 │
    │      EAU        │ 66.5 ─── Centre eau = Centre paroi
    │   (3 mm)        │
    │                 │
    ├─────────────────┤ 65.0 ─── Surface avant eau/paroi
    
    |▓▓▓|         |▓▓▓|  ← Paroi PVC (r = 10-11 mm)
```

---

## Problème 2 : WaterRings_passages avec 0 entrées

### Diagnostic

Le ntuple `WaterRings_passages` contenait 0 entrées alors que :
- De l'énergie était bien déposée dans l'eau (64130 keV)
- Le SensitiveDetector était correctement attaché aux volumes

### Cause

Dans `ScorePlane4SD.cc`, ligne 61 :
```cpp
const G4String targetName = "physScorePlane4";  // ← NOM INCORRECT !
```

Les volumes d'eau sont nommés `physWaterRing0`, `physWaterRing1`, etc., pas `physScorePlane4`.

La condition de détection ne matchait jamais car le nom du volume était différent.

### Solution

Remplacer la comparaison de nom par une fonction qui détecte les noms `physWaterRing*` :

**AVANT (incorrect) :**
```cpp
const G4String targetName = "physScorePlane4";

const bool enteringVolume = 
    ((!prePV) || (prePV->GetName() != targetName)) &&
    (postPV && postPV->GetName() == targetName);
```

**APRÈS (corrigé) :**
```cpp
inline bool IsWaterRingVolume(const G4String& volName) {
    return (volName.find("physWaterRing") != std::string::npos);
}

// Dans ProcessHits():
const G4String preName = prePV ? prePV->GetName() : "";
const G4String postName = postPV ? postPV->GetName() : "";

const bool enteringVolume = 
    (!IsWaterRingVolume(preName)) && IsWaterRingVolume(postName);
```

---

## Fichiers fournis

1. **`ScorePlane4SD_corrected.cc`**
   - Version complète corrigée de `ScorePlane4SD.cc`
   - Remplacer `src/ScorePlane4SD.cc` par ce fichier

2. **`DetectorConstruction_PVC_section_CORRECTED.cc`**
   - Section corrigée (lignes ~310-380)
   - Remplacer les lignes correspondantes dans `src/DetectorConstruction.cc`

3. **`PATCH_DetectorConstruction_PVC_overlap.txt`**
   - Explications détaillées du patch PVC

---

## Application des corrections

### Option 1 : Remplacement manuel

1. Copier `ScorePlane4SD_corrected.cc` vers `src/ScorePlane4SD.cc`
2. Éditer `src/DetectorConstruction.cc` et remplacer les lignes 324-336 par :

```cpp
        const G4double waterThickness = 3.0*mm;
        const G4double ringRadialThick = 2.0*mm;
        const G4double waterRadius = 10.0*mm;
        const G4double pvcThickness = 1.0*mm;
        const G4double containerWallHeight = waterThickness;  // CORRIGÉ
        
        const G4double zWaterBackSurface = 68.0*mm;
        const G4double zWaterFrontSurface = zWaterBackSurface - waterThickness;
        const G4double zWaterCenter = zWaterBackSurface - waterThickness/2.0;
        const G4double zPVCBottomCenter = zWaterBackSurface + pvcThickness/2.0;
        const G4double zPVCWallCenter = zWaterCenter;  // CORRIGÉ
```

### Option 2 : Script sed

```bash
# Correction ScorePlane4SD
cp ScorePlane4SD_corrected.cc src/ScorePlane4SD.cc

# Correction DetectorConstruction (lignes 328-329 et 336)
sed -i 's/const G4double containerHeight = 5.0\*mm;/\/\/ containerHeight supprimé/' src/DetectorConstruction.cc
sed -i 's/const G4double containerWallHeight = containerHeight - pvcThickness;/const G4double containerWallHeight = waterThickness;  \/\/ CORRIGÉ: 3mm au lieu de 4mm/' src/DetectorConstruction.cc
sed -i 's/const G4double zPVCWallCenter = zWaterFrontSurface + containerWallHeight\/2.0;/const G4double zPVCWallCenter = zWaterCenter;  \/\/ CORRIGÉ: 66.5mm au lieu de 67mm/' src/DetectorConstruction.cc
```

---

## Vérification après correction

Après recompilation et exécution, vérifier :

1. **Pas d'overlap** dans le log :
   ```
   Checking overlaps for volume physPVCWall:0 (G4Tubs) ... OK!
   ```

2. **WaterRings_passages non vide** :
   ```
   [ScorePlane4SD] WROTE row: pdg=22 name=gamma ...
   ```

3. **Comptage cohérent** :
   - Nombre d'entrées dans `WaterRings_passages` proche du nombre de photons atteignant z=65mm
   - Environ 10000-13000 entrées pour 1M d'événements
