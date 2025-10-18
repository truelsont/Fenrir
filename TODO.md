# TODO

## Phase 0: Foundation (Week 1-2)
- [ ] CMake setup (C++17, Boost, cross-platform)
- [X] Project structure (`src/compute/`, `src/engine/`, `src/game/`, `src/godot/`, `src/util/`)
- [ ] CPU backend stub (threadpool, empty task handlers)
- [ ] Data abstraction layer (`IDataProvider` interface)
- [ ] Standardized data schema (MarketSnapshot, AgentObservation, etc.)
- [ ] Godot GDExtension skeleton (`GameNode` class)
- [ ] Test: Call C++ from Godot, submit compute task
- [ ] Hello World from cpp engine code

## Phase 1: Map Generation (Week 3-4)
- [ ] Compute facade interface (`IComputeBackend`, task-based API)
- [ ] Heightmap generation (Perlin noise via compute facade)
- [ ] Voronoi provinces (Lloyd's relaxation via compute facade)
- [ ] Resource assignment (iron, wheat, etc. per province)
- [ ] Godot rendering (colored tiles for provinces)
- [ ] Unit tests for sanity generation 
- [ ] Better Spec for map 
- [ ] Determisitc generation and compare when serialized
- [ ] One UI tool chain call to generat map 

## Phase 2: Economy Core (Week 5-7)
- [ ] Define goods (wheat, iron, tools)
- [ ] Define agent types (farmer, miner, craftsman)
- [ ] Market clearing algorithm (supply/demand → prices)
- [ ] Production system (agents produce goods)
- [ ] Consumption system (agents consume goods)
- [ ] SimulatedDataProvider implementation
- [ ] Agent observation system (agents see markets through IDataProvider)
- [ ] Test: Prices respond to supply/demand changes
- [ ] Test: Agents make decisions from observations only

## Phase 3: Economy Visualization (Week 8)
- [ ] Expose market data to Godot (prices, volumes)
- [ ] UI: Price charts, production stats
- [ ] Visualization: Color provinces by wealth/production
- [ ] Time controls (pause, speed, step)
- [ ] Test: Observable cause-effect (drought → price spike)

## Phase 4: Political System (Week 9-10)
- [ ] Nations (own provinces, have treasury)
- [ ] Taxation (% of production goes to nation)
- [ ] Government spending (infrastructure, military budget)
- [ ] Basic diplomacy (peace/war state)
- [ ] Test: Taxation affects economic growth

## Phase 5: Cultural System (Week 11-12)
- [ ] POPs (population groups: culture, class, location)
- [ ] POP needs (food, goods consumption)
- [ ] POP employment (work in farms, mines, factories)
- [ ] Culture spread (migration, assimilation)
- [ ] Test: POP growth/decline affects production

## Phase 6: Military System (Week 13-14)
- [ ] Military units (infantry, cavalry, artillery)
- [ ] Recruitment (costs money, uses POPs)
- [ ] Supply system (armies need food, equipment)
- [ ] Combat resolution (battle outcomes)
- [ ] Test: War disrupts economy (casualties, supply)

## Phase 7: Player Interaction (Week 15-16)
- [ ] Player nation selection
- [ ] Player controls (taxes, diplomacy, military orders)
- [ ] Victory conditions (economic, military, diplomatic)
- [ ] Save/load system (serialize WorldState)
- [ ] Test: Play from start to victory

## Phase 8: GPU Acceleration (Week 17-18)
- [ ] Profile bottlenecks (market clearing, agent updates, pathfinding)
- [ ] Vulkan backend implementation
- [ ] Data layout optimization (SoA for GPU)
- [ ] Benchmark CPU vs GPU
- [ ] Test: Same results, 10x+ speedup

## Phase 9: Agent Learning (Week 19-21)
- [ ] LibTorch C++ integration
- [ ] Add ML tasks to compute facade (AGENT_TRAIN, AGENT_INFERENCE)
- [ ] Feature extraction from observations
- [ ] Neural network architecture for agent decision-making
- [ ] Training pipeline (historical data → trained model)
- [ ] Inference pipeline (observation → action)
- [ ] Model serialization (save/load trained agents)
- [ ] Test: Agents learn profitable trading strategies

## Phase 10: Real-World Data Integration (Week 22-24)
- [ ] CSV data adapter (load historical market data)
- [ ] API data adapter (fetch live data from external APIs)
- [ ] Database adapter (query SQL for historical data)
- [ ] RealWorldDataProvider implementation
- [ ] HybridDataProvider (mix simulated + real data)
- [ ] Validation system (compare simulation vs reality)
- [ ] Calibration system (tune simulation to match real data)
- [ ] Test: Train agent on real-world data, test in simulation
- [ ] Test: Simulation matches real-world market behavior

## Research & Design
- [ ] Economic model research (Victoria 2 POPs, agent-based models)
- [ ] SysML diagrams (system architecture, data flow)
- [ ] Vulkan compute shader examples
- [ ] CUDA vs Vulkan vs Metal comparison
- [ ] LibTorch C++ API evaluation
- [ ] Real-world data sources (commodity prices, stock markets, economic indicators)
- [ ] Agent learning architectures (DQN, PPO, A3C for economic agents)
- [ ] Digital twin design patterns
- [ ] Time series forecasting methods

## Infrastructure
- [ ] Logging setup (Boost.Log)
- [ ] Unit test framework (Catch2 or Google Test)
- [ ] CI/CD (GitHub Actions for Linux + macOS)
- [ ] Documentation (Doxygen or similar)
- [ ] Performance profiling tools

## Backlog (Post-MVP)
- [ ] More goods (100+ commodities)
- [ ] Complex production chains (iron → tools → machines)
- [ ] Trade routes (land, sea, rail)
- [ ] Technology system (unlock new production methods)
- [ ] Multiplayer (deterministic lockstep)
