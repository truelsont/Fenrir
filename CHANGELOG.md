# Changelog

## 2025-10-17 - Project Reorganization

### Changed
- **Adopted Google C++ Style Guide**
  - File extensions: `.hpp` → `.hh`, `.cpp` → `.cc`
  - Naming: `snake_case_` for private members, `kPascalCase` for constants
  - Formatting: 2-space indent, 80-char lines
  - Added `.clang-format` configuration

- **Reorganized Source Structure**
  - Moved Godot integration: `src/*.{hpp,cpp}` → `src/godot/*.{hh,cc}`
  - Separated concerns: Godot code isolated in `src/godot/`
  - Updated CMakeLists.txt to reflect new paths
  - Updated include paths to use full project path

- **Separated Godot Project Files**
  - Moved Godot files to `godot_project/` directory
  - Updated `fenrir.gdextension` to point to `res://../bin/libfenrir.dylib`
  - Cleaner separation between engine code and game project

- **Documentation Consolidation**
  - Merged `QUICKSTART.md`, `BUILD_INSTRUCTIONS.md`, `REBUILD_FOR_GODOT_4.4.md` → `docs/SETUP.md`
  - Added `docs/STYLE_GUIDE.md` for C++ coding standards
  - Updated `docs/ARCHITECTURE.md` with StateManager pattern
  - Compressed README.md, moved details to docs/

### Added
- `.clang-format` - Google C++ style configuration
- `docs/STYLE_GUIDE.md` - Comprehensive C++ coding standards
- `docs/ARCHITECTURE.md` - StateManager as central coordinator pattern
- `STATUS.md` - Project status and quick reference
- `CHANGELOG.md` - This file

### File Moves
```
src/game_node.hpp       → src/godot/game_node.hh
src/game_node.cpp       → src/godot/game_node.cc
src/register_types.cpp  → src/godot/register_types.cc
project.godot           → godot_project/project.godot
fenrir.gdextension      → godot_project/fenrir.gdextension
main.gd                 → godot_project/main.gd
main.tscn               → godot_project/main.tscn
icon.svg                → godot_project/icon.svg
```

### Technical Details
- Namespace structure: `fenrir::godot::GameNode` (not `fenrir::GameNode`)
- Include guards: `FENRIR_SRC_GODOT_GAME_NODE_HH_` format
- Includes: Use full path `"src/godot/game_node.hh"` (not relative)
- Member variables: Trailing underscore `tick_count_` (not `tick_count`)

### Build Status
- ✅ Builds successfully with new structure
- ✅ Godot 4.4 integration working
- ✅ All paths updated correctly

## 2025-10-17 - Initial MVP Setup

### Added
- CMake build system
- Godot 4.4 GDExtension integration
- Basic GameNode class (tick counter, hello world)
- Automated build script with Python venv
- Documentation structure

### Completed
- Phase 0: Foundation ✅
