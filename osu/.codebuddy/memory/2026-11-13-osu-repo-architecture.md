# 调研报告 A：osu! (lazer) 全仓架构（2026-11-13）

> 对象 `p:/Dev/osu`，静态分析。路径相对仓库根，行号实测。版本判定见 `INDEX.md`。

---

## A1. 定量画像

| 指标 | 实测 |
|---|---|
| 文件总数（排除 obj/bin/.git） | 5,656 |
| C# 源文件 / 行数 | 4,928 / **602,717** |
| 项目 | 35 个 `.csproj`，`osu.sln` 1 + `.slnf` 3 |
| 资源 | `.png` 174、`.osu` 163、`.json` 119、`.osk` 35、`.ini` 21、`.osb` 14 |
| 测试标注 | 约 6,075 处 `[Test]/[TestCase]/[TestFixture]` |
| `// TODO/FIXME/HACK` | 149 处（140+ 文件，高度分散） |

按项目分布：`osu.Game` 2,776 `.cs`、`osu.Game.Tests` 787、四规则集合计 797、四规则集测试 315、Tournament 98、Desktop 18、Benchmarks 14、Android/iOS 各 4、移动端头项目各 1–2、Templates 65。最大的 12 个文件：`OsuGame.cs` 1,860 行、`Editor.cs` 1,718、`RealmAccess.cs` 1,532、`TestSceneHitObjectSampleAdjustments.cs` 1,496、`TestSceneScreenNavigation.cs` 1,440、`BeatmapImporterTests.cs` 1,366、`Player.cs` 1,323、`SongSelect.cs` 1,305、`LegacyBeatmapDecoderTest.cs` 1,297、`TestSceneMultiplayer.cs` 1,276、`MultiplayerClient.cs` 1,260、`BeatmapCarousel.cs` 1,235。

**"此快照不存在"校正**：无 `SongSelectV2`（唯一 V2 是 `Screens/RankingV2/`）、无 `RealmContextFactory`/`ArchiveModelManager`/`StatefulMultiplayerClient`/`OsuIntegrationTest`/`DefaultSkin`/`CreateConversionScoreReference`；`IniConfigManager` 来自 framework。旧资料勿套用。

## A2. 工程拓扑与构建

`osu.sln` 含全部项目；开发/CI 一律用 `.slnf`（`README.md:77-83`）：`osu.Desktop.slnf:5-27` 含 **22 个项目**（产品库、4 规则集、6 桌面测试、Benchmarks、8 模板工程）；`osu.Android.slnf:5-16` / `osu.iOS.slnf:5-16` 只做平台编译校验。

依赖严格单向：`osu.Framework(NuGet) ← osu.Game ← {4 Rulesets, Tournament} ← {Desktop/Android/iOS, Tests, Benchmarks, Templates}`。规则集彼此完全不引用（`osu.Game.Rulesets.Osu.csproj:3,16` 只引用 `osu.Game`），由 `osu.Desktop.csproj:26-31` 一次性装配。

`osu.Game.csproj:20-49` 关键包：`ppy.osu.Framework 2026.1111.0`、`Realm 20.1.0`（Fody weaving，`FodyWeavers.xml:1-3` 关遥测）、`Microsoft.AspNetCore.SignalR.Client 10.0.13` + MessagePack/NewtonsoftJson 协议、`MessagePack 3.1.12`、`SharpCompress 0.50.4`、`TagLibSharp 2.3.0`、`Sentry 6.11.0`、`Humanizer 3.0.10`、`AutoMapper 13.0.1`、`DiffPlex 1.9.0`、**`NUnit 4.6.1` 被产品程序集直接引用**（测试基建编译进 `osu.Game`）。`Directory.Build.props` 统一 C#14/nullable/XML 文档、`Resources/**` 嵌入（`:21-23`）、BannedApiAnalyzers + `BannedSymbols.txt` + `osu.globalconfig`、9 类 `AnalysisMode`。

构建/检查：`dotnet run --project osu.Desktop`（`README.md:92`）、`dotnet build -c Debug -warnaserror osu.Desktop.slnf -p:EnforceCodeStyleInBuild=true`、`.\InspectCode.ps1`（tool restore → clean → 风格构建 → `dotnet CodeFileSanity` → `jb inspectcode --no-build`，工具版本锁在 `.config/dotnet-tools.json`）、`dotnet format`。`UseLocalFramework.ps1` / `UseLocalResources.ps1` 把相邻目录 framework/resources 源码临时换成本地项目引用。

## A3. framework 边界（理解全仓第一把钥匙）

`ppy.osu.Framework` 提供引擎层：`GameHost`/`DesktopGameHost`/`AndroidGameActivity`/`GameApplicationDelegate`、`Framework.Game`、`Drawable`/`CompositeDrawable`/`Container`/`OverlayContainer`、`Screen`/`ScreenStack`、`AudioManager`/`Track`、`Storage`、`Bindable`/`LeasedBindable`、`DependencyContainer` + `[Cached]`/`[Resolved]`/`[BackgroundDependencyLoader]`、`LoadComponentAsync`/`Scheduler`、`TextureStore`/`ShaderManager`、`ILocalisationStore`、`IRenderer`/`DrawNode`/`IVertexBatch`/`BufferedContainer`/`Blur`/`EdgeEffect`/`BlendingParameters`。

本仓库自定义领域层：`OsuGame(Base)` 的全局 DI 图与导入/更新管线、`OsuScreen` + 三层栈 + 租借 bindable、`OverlayActivation`/`IOverlayManager`、Realm 访问封装、谱面/规则集域模型、UI 组件库、皮肤系统、在线客户端、平台子类。调用链常不是显式构造参数而是容器解析。

## A4. 启动链路与 DI 总装配

- **Desktop**（唯一静态 `Main`）`osu.Desktop/Program.cs:34`：Velopack 钩子 `:39,181-209` → Windows 版本/NVIDIA/正则超时检查 `:41-72` → `Host.GetSuitableDesktopHost(..., IPCPipeName = OsuGame.IPC_PIPE_NAME)` `:105-111` → 非主实例把 `osu://`/文件导入经 IPC 转发后退出 `:113-124,153-179` → `host.Run(new OsuGameDesktop(args))` `:144`（`--tournament` 时 `TournamentGame` `:141`）。`OsuGameDesktop.cs:28` 覆写 `VelopackUpdateManager` `:109`、Discord RPC / WinKeyBlocker / WebSocket 数据源 `:134-164`。
- **Android** 无 `Main`：`OsuGameActivity.cs:53`，`[IntentFilter]` `:22-52` 声明 `.osz/.osk/.osr` 与 `osu://`；`OnCreate():83-118` **手工 `Assembly.Load` 四个规则集** `:114-117`（AOT 下反射扫不到）；`handleIntent():122-162` → `game.Import(ImportTask[])`。
- **iOS** `osu.iOS/Program.cs:10-13` → `AppDelegate.cs:42` `CreateGame() => new OsuGameIOS(this)`；`OsuGameIOS.cs:19` 覆写 Storage/安全区/朝向/更新器。

`OsuGameBase.cs:77-78`（`[Cached]`, `partial`, `ICanAcceptFiles`/`IBeatSyncProvider`）只管依赖不画 UI，专供测试宿主；`OsuGame.cs:98-99` 叠加菜单/屏幕栈/全局 overlay/快捷键。DI 两层容器（`OsuGameBase.cs:537-538`、`OsuGame.cs:356-359`）。`OsuGameBase.cs:266-445` 的 `[BackgroundDependencyLoader] load()` 是全仓最重要的装配清单：`RealmAccess` `:283`、`RealmRulesetStore` `:285`、`Storage` `:290`、`LocalConfig` `:298`、`SkinManager` `:307`、`APIAccess` `:323`、`BeatmapDifficultyCache`/`ScoreManager`/`BeatmapManager`/`ModelDownloader` `:327-336`、`BeatmapUpdater`/`SpectatorClient`/`MultiplayerClient`/`MetadataClient` `:342-345`、`SessionStatics`/`OsuColour` `:363`、`LeaderboardManager` `:381`、`MusicController` `:398`、`RealmKeyBindingStore` `:432`；画面骨架 `SafeArea → ScalingContainer → GlobalActionContainer → GlobalCursorDisplay → TooltipContainer` `:410-427`。

`OsuGameBase.Importing.cs:20-55` 是同 `partial class` 的导入面（handler 后注册优先、`Task.WhenAll` 按扩展名分组）。`OsuGame.cs:1560-1614` `loadComponentSingleFile<T>()` 是核心异步模式：**先 `CacheAs` 让组件未加载完即可 Resolve，再以 `Task.Run` 串行加载链、在 update 线程 `Scheduler.Add(LoadComponentAsync)`**。`Loader.cs:24` 并行预编译 shader / intro / spinner 后 Push `MainMenu`。`PerformFromMenuRunner.cs:22` 以 200ms 轮询递归沿 `GetParentScreen()`/`IHasSubScreenStack` 找目标屏 `:86-129` 并处理确认框阻塞 `:136-184`。

## A5. UI 架构骨架（详见报告 B/C/D）

`Screens/` 749 文件按 Menu / Select / Play / Edit / OnlinePlay（Multiplayer、Matchmaking、Playlists、Lounge、DailyChallenge）/ Ranking / RankingV2 / Import / Utility / Backgrounds / Footer / Spectate 组织。`OsuScreen.cs:25` 用策略属性（`AllowUserExit`/`HideOverlaysOnEnter`/`DisallowExternalBeatmapRulesetChanges` `:40-84`）与 bindable 控行为；`OsuScreenDependencies.cs:23-57` 对全局 Beatmap/Ruleset/Mods 做 `BeginLease()` 或 `GetBoundCopy()` —— **gameplay 期间外部改谱面被禁止的机制**；`OsuScreenStack.cs:11` 缓存 `BackgroundScreenStack`（`IEquatable` 去重）并加视差。

`Overlays/` 523 文件。`OsuGame.cs:1148-1158` 按语义分 5 个叠层容器；`:1269-1279` 互斥列表；`:1440-1449` `showOverlayAboveOthers()` 用 `Depth = -Clock.CurrentTime` 提层；`OverlayActivation` 三档（`Overlays/OverlayActivation.cs:6-11`）。`Graphics/` 214 文件（组件库/容器/频谱/粒子）、`Input/Bindings/GlobalActionContainer.cs:15` 按 global>editor>ingame>songselect>audio>overlay 优先级拼键位、`Localisation/` 99 个 `*Strings.cs`。

## A6. 游戏逻辑主干

```
.osu/.osr → Decoder/LegacyBeatmapDecoder → BeatmapInfo/BeatmapSetInfo (Realm)
  → WorkingBeatmap.GetPlayableBeatmap(ruleset, mods)
      转换 → mod 改写 → ApplyDefaults → BeatmapProcessor
  → Player 构造 DrawableRuleset + FrameStabilityContainer
  → 输入/回放帧 → DrawableHitObject.UpdateResult → HitWindows.ResultFor
  → JudgementResult → HealthProcessor / ScoreProcessor
  → PopulateScore(ScoreInfo) + ReplayRecorder(Replay)
  → LegacyScoreEncoder(.osr) → ScoreManager/ScoreImporter → Realm
```

- **谱面域** `Beatmaps/`（133）：Realm 元数据层（`BeatmapInfo.cs:29`、`BeatmapSetInfo.cs:20`、`BeatmapMetadata.cs:29`、`BeatmapDifficulty.cs:9` 为 EmbeddedObject）与运行时 `Beatmap<T>` `Beatmap.cs:17`（ControlPointInfo 二分查找 `ControlPointInfo.cs:21,75`）。`Formats/Decoder.cs:13,54` 按 `osu file format vN` 分派，`LegacyBeatmapDecoder.cs:25` 主解码（v<5 有 24ms 偏移）；`BeatmapManager.cs:39` + `WorkingBeatmapCache.cs:31` 按需解码。
- **转换链** `WorkingBeatmap.cs:277-351`：`CreateBeatmapConverter → CanConvert → IApplicableToBeatmapConverter → Convert() → IApplicableAfterBeatmapConversion → IApplicableToDifficulty → PreProcess → ApplyDefaults → IApplicableToHitObject → PostProcess → IApplicableToBeatmap`。`DifficultyCalculator.cs:24` 复用该链后跑 `CreateSkills/CreateDifficultyHitObjects/CreateDifficultyAttributes`，缓存入口 `BeatmapDifficultyCache.cs:36,166`。
- **Ruleset 抽象** `Rulesets/`（270）：`Ruleset.cs:40` 是唯一总工厂（抽象 `GetModsFor` `:133`、`CreateDrawableRulesetWith` `:251`、`CreateBeatmapConverter` `:270`、`CreateDifficultyCalculator` `:279`；虚工厂覆盖 Score/Health/BeatmapProcessor、PerformanceCalculator、HitObjectComposer、BeatmapVerifier、ScoreMultiplierCalculator、SkinTransformer、Config）。`RulesetInfo.CreateInstance()` 靠 `InstantiationInfo` 反射；`RulesetStore.cs:88,140` `Assembly.LoadFrom` 扫 DLL；`RealmRulesetStore.cs:92` 校验 `CURRENT_RULESET_API_VERSION = "2026.818.0"` 并预跑探活，崩溃 DLL 改名 `.dll.broken`；`ILegacyRuleset.cs:8` 提供 LegacyID 与 stable mod/分数互转。
- **Mod 体系**：`IApplicableTo*` 接口能力注入，序列化走 `Online/API/APIMod.cs`，校验 `Utils/ModUtils.cs:21`，`ModPreset.cs:20` 是 Realm 对象；`Mod.ScoreMultiplier` 已 Obsolete，改由 `CreateScoreMultiplierCalculator()`。
- **判定时序**：`Screens/Play/GameplayClockContainer.cs:19` 缓存 `IGameplayClock`；`Rulesets/UI/FrameStabilityContainer.cs:25` 把 Playfield + 输入 + overlay 置于 `IFrameStableClock`，`applyFrameStability():127-209` 钳到 60fps 步长、catch-up 上限 10ms；`RulesetInputManager.cs:28` 同时处理真实按键与 `ReplayStateChangeEvent`。`DrawableHitObject.UpdateResult() → CheckForResult() → HitWindows.ResultFor() → ApplyResult() → Playfield.NewResult → DrawableRuleset.NewResult → Player → Health/ScoreProcessor.ApplyResult`（支持 `RevertResult` 倒带）。`ScoreProcessor.cs:239-285` 累加 combo/accuracy/base score；`HealthProcessor.cs:54-98` 判 FailConditions。`ReplayFrame.cs:10` 用 MessagePack `[Key(0)]`，`ReplayRecorder.cs:21` 60fps 录制并推观战。
- **成绩** `Score.cs:11` = `ScoreInfo + Replay`；`ScoreInfo.cs:29` 是 Realm `Score` 表（`ModsJson` 经 APIMod 反序列化、`[Backlink]` 关联谱面）；`Scoring/Legacy/LegacyScoreDecoder.cs:31` 处理 `.osr` LZMA 帧、旧 mod 补 `ModClassic`、`StandardisedScoreMigrationTools` 计分版本迁移。
- **编辑器** `Screens/Edit/Editor.cs:69` + `EditorBeatmap.cs:27`（TransactionalCommitComponent）+ `Rulesets/Edit/HitObjectComposer.cs:47`（蓝图/snap/选中）+ `BeatmapVerifier.cs:14` + 30+ `Checks/Check*.cs`。

## A7. 四规则集对照

| | osu! | Taiko | Catch | Mania |
|---|---|---|---|---|
| 入口 | `OsuRuleset.cs:53` | `TaikoRuleset.cs:50` | `CatchRuleset.cs:46`（ShortName `"fruits"`） | `ManiaRuleset.cs:49` |
| 物件 | HitCircle/Slider/Spinner | Hit(强拍)/DrumRoll/Swell | Fruit/JuiceStream/BananaShower(+Droplet) | Note/HoldNote |
| 坐标模型 | 2D 自由 512×384 `OsuPlayfield.cs:47` | 一维横滚 + 红蓝鼓面 | X 一维 + 下落接果器 | 离散列（≤10 键）+ 单/双 stage |
| 判定档 | Great/Ok/Meh/Miss + tick | Great/Ok/Miss | Great/Miss + droplet | **6 档** Perfect→Meh/Miss |
| 命中窗口 | `OsuHitWindows.cs:12` 80/50/20ms | `TaikoHitWindows.cs:12` 50/35/20ms | — | `ManiaHitWindows.cs:12` 22.4ms 起、不受倍速影响 |
| 难度技能 | Aim/Speed/Flashlight/Reading | Colour/Reading/Rhythm/Stamina | 仅 Movement | Strain |
| 特色算法 | 堆叠 `OsuBeatmapProcessor.cs:16`、Autoplay 覆写 `Generate()` | SV→滚动速度、Swell 轮敲 | HyperDash `CatchBeatmapProcessor.cs:212` | `LegacyRandom` + 4 种 PatternGenerator |
| 健康处理器 | Draining | **Accumulating** | LegacyDraining | LegacyDraining |
| Mod 文件数 | 48 | 22 | 22 | 40 |

共享 HUD 在 `Screens/Play/HUD/` 与 `Skinning/Legacy*`，规则集只注册皮肤转换器与少量专属件。`Templates/` 把契约产品化：`dotnet new ruleset|ruleset-scrolling|ruleset-example|ruleset-scrolling-example`（`Templates/README.md:7-20`），`PippidonRuleset.cs:18-54` 是最小样例（强制保留 `RulesetAPIVersionSupported` `:54`）。

## A8. 持久化 / 配置 / 在线 / 周边

- **Realm** `Database/RealmAccess.cs:46` 唯一工厂与上下文守卫：`schema_version = 52` `:106`，`:57-105` 逐版迁移日志，`:829/:837` 执行迁移，`:233-272` 开发版版本化拷贝与回退；**主线程约束** `Realm` 属性 `:167` 仅更新线程可用（`:176` 显式抛错），`Run/Write` `:454/:471/:514/:534`、`RunAsync/WriteAsync` `:489/:554` 用 CountdownEvent 管在途任务，`BlockAllOperations():1379` 用于迁移；订阅必须走 `RealmObjectExtensions.cs:296-365` 的 `QueryAsyncWithNotifications`（原生 `SubscribeForNotifications` 被 `BannedSymbols.txt:10-13` 禁用，RS0030=error）；`Live.cs:13` / `RealmLive.cs:17,91` 禁止托管对象逃出 `PerformRead`。
- **导入框架**：`IModelImporter.cs:15` → `RealmArchiveModelImporter.cs:30`（单线程 `ThreadedTaskScheduler` `:52`、`Parallel.ForEachAsync` `:125`、路径穿越校验 `:375`、两段 `BeginWrite`）→ `ModelManager<T>.cs:20` → Beatmap/Score/Skin 三套 Manager/Importer。`RealmFileStore.cs:21` SHA-2 去重 + 硬链接；`BackgroundDataStoreProcessor.cs:37` 后台重算星数/补统计/分数标准化。
- **IO/配置**：`IO/OsuStorage.cs:12`、`ZipArchiveReader.cs:23,40,49`（单条目 200MB 上限、Shift-JIS 932 兼容 stable）、`LineBufferedReader.cs:14`、`Utils/FilesystemSanityCheckHelpers.cs:9` 防路径穿越。配置三层：`OsuConfigManager.cs:31`（键名不可改 `:333-334`）、`SessionStatics.cs:20`（内存）、`RealmRulesetSetting.cs:9`（规则集配置）。
- **在线** `Online/`（359）：REST `APIAccess.cs:34` 独立后台线程状态机 + `OAuth.cs:16` token 刷新 + `APIRequest.cs:69,75` 固定 `{APIUrl}/api/v2/{Target}`，`TriggerSuccess/Failure` 经 `API.Schedule` 回更新线程 `:184/:199`；SignalR `EndpointConfiguration.cs:29-49` 四端点 + `HubClientConnector.cs:15` / `PersistentEndpointClientConnector.cs:15,39` 指数退避重连 + 三个业务客户端（`MultiplayerClient.cs:33` 房间状态机、`SpectatorClient.cs:29` 200ms 帧包、`MetadataClient.cs:19` 谱面元数据增量 → `BeatmapOnlineChangeIngest.cs:20` → `BeatmapUpdater.cs:17`）；另有 WebSocket 聊天/通知、排行榜/下载追踪。
- **皮肤** `Skinning/`（86）：`ISkin.cs:15-55` 四类查找、`Skin.cs:31` 解析 `skin.ini` 与 `{target}.json`、`SkinManager.cs:39,104-111` 缓存 Retro/DefaultLegacy/Triangles/**Argon（默认）**/ArgonPro、`SkinnableDrawable`/`SkinProvidingContainer`/`SkinTransformer` 带回退查找链、`LegacySkin.cs:34` 兼容 stable。
- **IPC/Updater**：`ArchiveImportIPCChannel.cs:15` + `OsuSchemeLinkIPCChannel.cs:11` 单实例转发；`IPC/WebSocketServer.cs:45` 只听 127.0.0.1；`Updater/UpdateManager.cs:29` + Desktop `VelopackUpdateManager.cs:20` / 移动 `MobileUpdateNotifier.cs:24`。

## A9. 外围项目

- `osu.Game.Tournament/`（98）：赛事导播客户端，仅依赖 `osu.Game`，`--tournament` 启动。数据不是数据库而是 `bracket.json`（`TournamentGameBase.cs:32,101,331` 读写 `LadderInfo`）；与 stable 用**文件 IPC** 轮询 `ipc.txt`/`ipc-state.txt`/`ipc-scores.txt`（`IPC/FileBasedIPC.cs:22,65-67`）；`TournamentSceneManager.cs:93-105` 列 13 个导播屏。
- 平台：`osu.Desktop`（net10.0 WinExe、程序集名 `osu!`、Velopack 1.2.0 + Discord RPC + `Windows/WindowsAssociationManager.cs:17` 文件/URI 关联）；`osu.Android`（net10.0-android，minSdk 24/targetSdk 36）；`osu.iOS`（net10.0-ios，最低 15.0，`Info.plist` 4 种文档 UTI + URL Scheme）。
- `osu.Game.Benchmarks/`：BenchmarkDotNet 0.15.8，`BenchmarkTest.cs:10-21` 让同一用例既可 CLI 又可 NUnit；13 个用例覆盖谱面解析、转盘过滤、四规则集难度计算、HitObject 池化、Realm 读取、Mod 反射、分数倍率等，**无 UI/DrawNode 绘制专项**。

## A10. 四条端到端流程

1. **选歌→游玩→结算**：`FilterControl.cs:289-324`（`FilterQueryParser` 解析 `stars=/ar=/bpm=`）→ `SongSelect.cs:888,898` 250ms 防抖 → `BeatmapCarousel.cs:103-107` Matching→Sorting→Grouping → `SongSelect.cs:558 SelectAndRun` → 抽象 `OnStart` `:355` → `SoloSongSelect.cs:99-129` `Push(new PlayerLoader(...))` → `PlayerLoader.cs:549-728` 异步准备 → `Player.cs:229/1115/1171` → `:861-942 progressToResults` → `SubmittingPlayer.cs:195-341` 单次提交（校验速率/token/命中数）+ `SpectatorClient.cs:202,393,417` 观战帧 → `ResultsScreen`。全局 **solo/multiplayer/playlists/daily challenge 四套入口**共用 `PlayerLoader → Player → SubmittingPlayer` 模板方法链。
2. **在线多人**：`OnlinePlayScreen.cs:87` → `MultiplayerClient.cs:237/258/484/514/863` 状态机 → `MultiplayerMatchSubScreen.cs:558 onLoadRequested` 按 `LocalUser.State` 分叉观战或 `MultiplayerPlayerLoader` → `MultiplayerPlayer.cs:215` 最多等 60s 汇总成绩。Matchmaking 是 98 文件独立子模块自成 UI 栈。
3. **编辑器**：`Editor.cs:587-609 Save()` → `BeatmapManager.Save(...)` + `lastSavedHash`；`EditorBeatmap.cs:333-434` 事务；撤销 `EditorChangeHandler.cs:57-107` 存 byte[] 快照（上限 50），`BeatmapEditorChangeHandler.cs:31-38` 用 `LegacyBeatmapEncoder` 序列化整图 + `LegacyEditorBeatmapPatcher` 打文本补丁 —— **undo 建立在 legacy 文本 diff 上**。
4. **文件导入**：拖拽/`osu://`/Intent → `OsuGameBase.Importing.cs:28-55` → `ImportTask.cs:15,47` 选 ArchiveReader → `RealmArchiveModelImporter.cs:101-403` hash 跳过/路径校验/两段写事务 → ProgressNotification。

## A11. 测试与质量保障

「底座共享 + 三种可执行形态 + 平台编译校验」：测试基建**直接编译进产品程序集** `osu.Game/Tests/`（51 `.cs`），`osu.TestProject.props:3,9,14` 把 `StartupObject` 设为 `VisualTestRunner` → `dotnet run --project osu.Game.Tests` 打开交互式可视化测试浏览器，同一批 `TestScene` 又被 NUnit 无头重放。`osu.Game/Tests/Visual/OsuTestScene.cs:41` 注入临时 `RealmAccess`/`LocalStorage`/`DummyAPIAccess`/`TestRulesetConfigCache`、虚拟时钟 `TrackVirtualManual`（`:393-566`）。三类测试：`NonVisual/`（约 40+ `[HeadlessTest]`）、`Visual/**`、`Navigation/`（完整 `OsuGame` 集成，20 用例）。回归数据 `*-expected-conversion.json`（`osu.Game/Tests/Beatmaps/BeatmapConversionTest.cs:161-190`）、星数 `[TestCase]`（`OsuDifficultyCalculatorTest.cs:20-38`，精度 1e-5）、PP/SR 由 `.github/workflows/diffcalc.yml` 生成对比表。

CI `.github/workflows/ci.yml`：`inspect-code`（`-warnaserror` 风格构建 + `dotnet codefilesanity` + ReSharper InspectCode `:37-61`）；`test` 为 **Windows × Linux × SingleThread/MultiThreaded 四矩阵**（`OSU_EXECUTION_MODE` `:67,76,91`，超时 120min）；`:130-176` 仅编译 Android/iOS。`CONTRIBUTING.md:5-7` AI 贡献禁令、`:71-73` 测试覆盖 + InspectCode + **禁止 Release 跑测试**、`:79-80` 星数/PP 改动走 `pp-dev` 分支。

## A12. 架构评估

**优点**：① 插件契约干净（规则集只依赖 osu.Game + API 版本闸门 + 官方模板）；② 状态用 bindable 单向流动 + 屏幕 lease，跨线程用 `API.Schedule`/`Scheduler` 收敛；③ 可复现 gameplay（帧稳定时钟 + 共用 JudgementResult + RevertResult 倒带）；④ 危险 Realm 操作封进一层并用编译期 BannedApi 强制；⑤ 测试基建产品化；⑥ 质量闸门前置（-warnaserror + 命名 error 级 + 四矩阵 CI）。

**风险/技术债**：① 巨类集中（前 7 个核心文件 1,200–1,860 行）；② 三条长期兼容链 —— `Legacy*.cs` **149 个**、Realm **schema 52 版迁移**、计分版本迁移；③ 隐式跨层耦合（`BeatmapCarousel.cs:446,832` 依赖已写回 Realm 的 StarRating 快照且 realm 字符串排序与 .NET 不一致被标 FOOTGUN；`:826-861` UI 层直接 `realm.Run`；`SongSelect.cs:1228`、`SubmittingPlayer.cs:215`、`Editor.cs:598` 都是 UI 直连数据层）；④ 编辑器撤销基于文本补丁；⑤ `#pragma` 抑制（`HitResult.cs:163-410` 跨 247 行；`OsuGame.cs:1400-1410` 保留过时类型并附「SUPER SUPER bad」注释）；⑥ Realm 主线程约束是隐性契约；⑦ .NET 10 / C# 14 / framework 2026.1111.0 前沿工具链；⑧ 149 处 TODO 广而浅。

**建议阅读顺序**：`osu.Desktop/Program.cs` → `OsuGameBase.cs:266-445` → `OsuGame.cs:1065-1335` → `Screens/OsuScreen.cs` + `OsuScreenStack.cs` → `Beatmaps/WorkingBeatmap.cs:277-351` → `Rulesets/Ruleset.cs` → `Rulesets/UI/DrawableRuleset.cs` + `FrameStabilityContainer.cs` → 任一规则集对照 `Templates/.../PippidonRuleset.cs` → `Database/RealmAccess.cs` → `Online/API/APIAccess.cs` + `HubClientConnector.cs`。动手前跑 Debug `-warnaserror` 构建与测试，改 gameplay/计分/难度须注意 `*-expected-conversion.json` 回归与 `pp-dev` 分支约定。
