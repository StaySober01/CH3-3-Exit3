# Exit 3 코드 가이드

이 문서는 `Exit 3` 프로젝트의 커밋 내역과 현재 `main` 브랜치의 코드를 기준으로 핵심 구조와 실행 흐름을 설명한다. 코드를 수정하거나 Blueprint 에셋을 추가할 때 어떤 클래스와 설정을 확인해야 하는지 빠르게 찾는 것을 목적으로 한다.

## 1. 현재 구현 범위

- Unreal Engine 5.6.1 기반 Windows 1인칭 게임
- Persistent Level의 S자 연결 통로와 동적으로 생성되는 직선형 Gameplay Level 사용
- Stage 1부터 Stage 3까지 진행
- 라운드마다 `Normal` 또는 `Anomaly`를 50% 확률로 결정
- 플레이어의 실제 이동 방향으로 정상/이상 선택 판정
- 정답이면 다음 Stage, 오답이면 Stage 1로 초기화
- Stage 3 정답 시 Game Clear
- 스테이지별 이상현상 1개 사용
- HUD, Pause Menu, Game Clear UI 구현

핵심 게임 규칙은 C++에서 관리하고, 레벨 배치와 이상현상의 시각적 효과 및 UI 레이아웃은 Blueprint에서 구성한다.

## 2. 주요 커밋과 구현 변화

| 커밋 | 핵심 내용 |
| --- | --- |
| `15827c2` | 기본 C++ 클래스와 프로젝트 골격 구성 |
| `960be87` | 1인칭 캐릭터, Enhanced Input, 걷기·달리기·점프 구현 |
| `4b87850` | Stage 진행, Normal/Anomaly 판정, 정답·오답 라운드 루프 구현 |
| `28e0c1c` | Gameplay Level의 동적 로딩·제거 생명주기 연결 |
| `9a8326a` | Side A/B 기반 S자 통로, 양방향 Gameplay Level 생성, 자연스러운 순간이동 구현 |
| `7f474f4` | 이상현상 검색·검증·선택·복원 시스템 완성 |
| `3127a6f` | 세 오브젝트 크기 변경 이상현상 구성 |
| `16abf42` | 스테이지별 이상현상 1개 구성으로 MVP 범위 확정 |
| `b40891b` | HUD, Pause Menu, Game Clear 화면과 PlayerController 연결 |
| `e8457e6` | Game Clear 위젯 에셋 이름 수정 |

## 3. 소스 구조

```text
Exit3/Source/Exit3
├─ Public
│  ├─ Core
│  │  ├─ Exit3Types.h
│  │  ├─ Exit3GameMode.h
│  │  └─ Exit3GameState.h
│  ├─ Gameplay
│  │  ├─ Exit3StageManager.h
│  │  ├─ Exit3LevelStreamManager.h
│  │  ├─ Exit3ChoiceTrigger.h
│  │  └─ Exit3PassageTransitionTrigger.h
│  ├─ Anomaly
│  │  └─ Exit3AnomalyActor.h
│  ├─ Player
│  │  ├─ Exit3Character.h
│  │  └─ Exit3PlayerController.h
│  ├─ UI
│  │  ├─ Exit3HUDWidget.h
│  │  ├─ Exit3PauseMenuWidget.h
│  │  └─ Exit3GameClearWidget.h
│  └─ Data
│     └─ Exit3StageData.h
└─ Private
   └─ 각 Public 클래스의 구현 파일
```

## 4. 핵심 데이터 타입

모든 공용 열거형은 `Exit3Types.h`에 정의되어 있다.

### `EExit3Stage`

현재 진행 단계를 나타낸다.

```cpp
None, Stage1, Stage2, Stage3
```

`None`은 유효하지 않은 설정을 검출하기 위한 값이고 실제 라운드는 Stage 1부터 시작한다.

### `EExit3RoundState`

게임이 정한 현재 라운드의 실제 상태다.

```cpp
Normal, Anomaly
```

### `EExit3PlayerDecision`

플레이어가 이동으로 제출한 답이다.

```cpp
Normal, Anomaly
```

`RoundState`와 `PlayerDecision`의 값이 같으면 정답이다.

### `EExit3PassageSide`

S자 연결 통로의 두 접합부를 구분한다.

```cpp
SideA, SideB
```

이 값은 단순 위치 이름이 아니라 현재 Gameplay Level의 입구, 플레이어가 나온 출구, 다음 Gameplay Level을 생성할 방향을 계산하는 기준이다.

## 5. 전체 런타임 구조

```text
BP_Exit3GameMode
    ├─ AExit3GameState: Stage, RoundState, GameClear 상태 보관
    ├─ AExit3StageManager: 이상현상 검색·선택·복원
    └─ AExit3LevelStreamManager: Gameplay Level 로딩·제거와 양쪽 통로 차단

BP_Exit3PlayerController
    ├─ WBP_GameHUD
    ├─ WBP_PauseMenu
    └─ WBP_GameClear

BP_Exit3Character
    └─ IMC_Player와 IA_Move/Look/Jump/Sprint
```

### Persistent Level에 존재하는 항목

- S자 연결 통로
- `AExit3StageManager` 인스턴스 1개
- `AExit3LevelStreamManager` 인스턴스 1개
- Side A/B 복귀 판정 Trigger
- PlayerStart

### Gameplay Level에 존재하는 항목

- 관찰 공간과 소품
- `AExit3PassageTransitionTrigger` 기반 Blueprint
- `AExit3AnomalyActor` 기반 이상현상 Blueprint 인스턴스
- 각 이상현상이 참조하는 대상 Actor

`LevelStreamManager`는 Gameplay Level에 중복 배치하지 않는다. Persistent Level의 한 인스턴스가 모든 라운드의 스트리밍을 담당한다.

## 6. 게임 시작과 라운드 준비 흐름

### 6.1 게임 시작

`AExit3GameMode::BeginPlay()`가 전체 실행의 시작점이다.

1. Persistent Level에서 `AExit3StageManager`를 검색한다.
2. `AExit3LevelStreamManager`를 검색한다.
3. `OnGameplayLevelReady`에 `FinishRoundPreparation()`을 연결한다.
4. `StartNewRun()`을 호출한다.

### 6.2 새 게임 시작

`StartNewRun()`은 다음 상태를 초기화한다.

- 최초 입구를 `InitialEntrySide`로 설정
- Game Clear 상태 해제
- 현재 Stage를 Stage 1로 설정
- `StartRound()` 호출

UI의 다시 시작 버튼은 상태 일부만 직접 초기화하지 않고 현재 Persistent Level 전체를 다시 로드한다. 따라서 플레이어 위치, 스트리밍 레벨, 이상현상 상태까지 함께 새로 생성된다.

### 6.3 새 라운드 준비

`StartRound()`의 순서는 다음과 같다.

1. `bDecisionLocked = true`로 판정을 잠근다.
2. `FMath::RandBool()`로 Normal/Anomaly를 결정한다.
3. 현재 입구 Side에 맞는 위치로 Gameplay Level 재생성을 요청한다.
4. 로딩 중 Side A/B의 `EntranceBlocker`를 모두 활성화한다.
5. 이전 Gameplay Level 인스턴스를 숨기고 언로드한다.
6. 새 Gameplay Level 인스턴스를 로드한다.
7. 새 레벨이 실제로 화면에 표시되면 `OnLevelShown` 이벤트가 발생한다.
8. Blocker를 해제하고 `OnGameplayLevelReady`를 방송한다.
9. `FinishRoundPreparation()`이 이상현상을 준비한다.
10. 준비가 끝난 뒤 `bDecisionLocked = false`로 플레이어 판정을 허용한다.

`OnLevelLoaded`가 아니라 `OnLevelShown`을 사용하는 것이 중요하다. 레벨 데이터가 로드되었더라도 Actor가 아직 월드 검색에 나타나지 않을 수 있기 때문이다.

## 7. 판정과 Stage 진행

### 7.1 이동으로 답을 결정하는 규칙

현재 라운드가 시작된 입구를 `EntrySide`, 플레이어가 통과한 출구를 `ExitSide`라고 한다.

| 조건 | 플레이어 판단 |
| --- | --- |
| `ExitSide == EntrySide` | 되돌아옴 → `Anomaly` |
| `ExitSide != EntrySide` | 끝까지 진행함 → `Normal` |

이 계산은 `AExit3GameMode::SubmitExitSide()`에서 수행한다.

### 7.2 정답 처리

`SubmitDecision()`은 플레이어의 판단과 실제 `RoundState`를 비교한다.

```text
PlayerDecision == RoundState
    ├─ true  → Stage 1 → 2 → 3 → Game Clear
    └─ false → Stage 1로 초기화
```

판정이 끝나고 Game Clear가 아니라면 즉시 다음 `StartRound()`를 호출한다. 새 Gameplay Level은 플레이어가 S자 통로를 이동하는 동안 준비된다.

### 7.3 중복 판정 방지

`bDecisionLocked`는 다음 상황을 막는다.

- 로딩 중 Trigger 진입
- 같은 프레임 또는 연속 Overlap에 의한 중복 정답 처리
- Game Clear 이후 추가 판정

Trigger 자체에도 `bConsumed`가 있어 하나의 진입을 한 번만 처리한다.

## 8. 핵심 클래스별 가이드

### 8.1 `AExit3GameMode`

파일:

- `Public/Core/Exit3GameMode.h`
- `Private/Core/Exit3GameMode.cpp`

프로젝트의 중앙 조정자다. 개별 시각 효과를 직접 처리하지 않고 GameState, StageManager, LevelStreamManager를 순서대로 호출한다.

주요 함수:

| 함수 | 책임 |
| --- | --- |
| `BeginPlay()` | 관리자 검색, 스트리밍 완료 이벤트 연결, 새 게임 시작 |
| `StartNewRun()` | Stage와 최초 Side 초기화 |
| `StartRound()` | 상태 무작위 결정과 Gameplay Level 재생성 |
| `FinishRoundPreparation()` | 이상현상 적용 후 판정 잠금 해제 |
| `SubmitExitSide()` | Entry/Exit Side를 Normal/Anomaly 판단으로 변환 |
| `SubmitDecision()` | 정답 비교, Stage 증가 또는 초기화 |
| `AdvanceStage()` | Stage 3 이후 Game Clear 설정 |

외부에서 사용할 수 있는 이벤트:

- `OnRoundStarted(Stage, RoundState)`
- `OnDecisionResolved(Decision, bCorrect, CurrentStage)`

Blueprint 연출이 필요하다면 핵심 판정 코드를 복제하지 말고 이 이벤트를 구독하는 방식이 안전하다.

### 8.2 `AExit3GameState`

파일:

- `Public/Core/Exit3GameState.h`
- `Private/Core/Exit3GameState.cpp`

현재 진행 상태의 단일 저장소다.

보관 상태:

- `CurrentStage`
- `RoundState`
- `bGameCleared`

방송 이벤트:

- `OnStageChanged`
- `OnGameCleared`

HUD와 Game Clear 화면은 GameMode를 계속 조회하지 않고 이 이벤트를 구독한다.

### 8.3 `AExit3LevelStreamManager`

파일:

- `Public/Gameplay/Exit3LevelStreamManager.h`
- `Private/Gameplay/Exit3LevelStreamManager.cpp`

Gameplay Level 인스턴스의 생명주기와 S자 통로 양쪽 접합부를 담당한다.

주요 설정:

| 속성 | 의미 |
| --- | --- |
| `GameplayLevel` | 반복 생성할 `L_GameplayMap` Soft Reference |
| `GameplayLevelTransform` | Side A에서 Gameplay Level의 로컬 원점이 놓일 월드 Transform |
| `GameplayLevelTransformSideB` | Side B에서 로컬 원점이 놓일 월드 Transform |
| `EntranceBlocker` | Side A 로딩 차단 Collider |
| `EntranceBlockerB` | Side B 로딩 차단 Collider |
| `TransitionDestinationA/B` | Gameplay 끝에서 순간이동할 Persistent 통로 기준점 |

두 Gameplay Transform은 `L_DefaultMap`의 월드 좌표를 기준으로 한다. 즉, Transform Location은 Gameplay Level 내부 Actor의 위치가 아니라 Gameplay Level의 로컬 원점이 Persistent World에서 놓일 위치다.

### 8.4 `AExit3ChoiceTrigger`

파일:

- `Public/Gameplay/Exit3ChoiceTrigger.h`
- `Private/Gameplay/Exit3ChoiceTrigger.cpp`

플레이어가 관찰 공간에서 같은 Side로 되돌아왔을 때 해당 Side를 GameMode에 제출한다.

주요 설정:

- `ExitSide`: Trigger가 속한 Side A 또는 B
- `ExitDirection`: 판정을 허용할 진행 방향
- `bRequireExitDirection`: 방향 검사 사용 여부
- `MinimumExitDirectionDot`: 속도와 Arrow 방향의 최소 내적값

Arrow가 실제 통로에서 나오는 방향과 반대라면 Overlap은 발생해도 판정되지 않는다.

### 8.5 `AExit3PassageTransitionTrigger`

파일:

- `Public/Gameplay/Exit3PassageTransitionTrigger.h`
- `Private/Gameplay/Exit3PassageTransitionTrigger.cpp`

플레이어가 Gameplay Level 끝까지 갔을 때 복제 코너 안에서 반대편 Persistent 통로로 이동시킨다.

이동 시 보존하는 값:

- `TransitionSource` 기준 상대 위치
- Actor 상대 회전
- Controller 시선 상대 회전
- 상대 이동 속도

목적지는 현재 Entry Side의 반대쪽 `TransitionDestination`이다. 이동 성공 후 반대쪽 Side를 제출하므로 이 행동은 `Normal` 판단이 된다.

자연스러운 전환을 위해 Source와 Destination Arrow의 방향, 높이, 통로 중심 정렬이 일치해야 한다.

### 8.6 `AExit3StageManager`

파일:

- `Public/Gameplay/Exit3StageManager.h`
- `Private/Gameplay/Exit3StageManager.cpp`

현재 월드에 존재하는 이상현상 Actor를 매 라운드 새로 검색한다. Gameplay Level 인스턴스가 라운드마다 교체되므로 이전 포인터 목록을 계속 사용하지 않는다.

`PrepareRound()`의 순서:

1. `RefreshAnomalyActors()`로 현재 인스턴스의 Actor 검색
2. 모든 후보에 `RestoreAnomaly()` 호출
3. Normal이면 종료
4. Anomaly이면 현재 Stage에 맞는 후보 필터링
5. 후보가 여러 개면 직전 선택 ID 제외
6. 한 개를 무작위 선택하고 활성화

유효 후보 조건:

- Actor가 유효함
- `bEnabled == true`
- `AnomalyId != None`
- `SupportedStage != None`
- 전체 월드에서 `AnomalyId`가 중복되지 않음

현재 프로젝트는 스테이지별 후보가 하나이므로 해당 Stage의 Anomaly 라운드에는 항상 그 하나가 선택된다.

### 8.7 `AExit3AnomalyActor`

파일:

- `Public/Anomaly/Exit3AnomalyActor.h`
- `Private/Anomaly/Exit3AnomalyActor.cpp`

모든 이상현상 Blueprint의 C++ 부모 클래스다.

수명 주기:

```text
최초 ActivateAnomaly
    → CaptureNormalState (인스턴스당 최초 1회)
    → ApplyAnomalyEffect
    → bApplied = true

RestoreAnomaly
    → bApplied가 true일 때만 RestoreNormalState
    → bApplied = false
```

Blueprint에서 구현할 이벤트:

| 이벤트 | 용도 |
| --- | --- |
| `CaptureNormalState` | 대상의 원래 위치, Scale, Hidden 상태 등을 저장 |
| `ApplyAnomalyEffect` | 크기 변경, 숨김 해제 등 이상 효과 적용 |
| `RestoreNormalState` | 저장한 정상 상태로 복원 |

`DestroyActor`는 복원할 수 없으므로 오브젝트 제거 이상현상에서는 사용하지 않는다. 대신 `Set Actor Hidden In Game`과 `Set Actor Enable Collision`을 함께 사용한다.

### 8.8 `AExit3Character`

파일:

- `Public/Player/Exit3Character.h`
- `Private/Player/Exit3Character.cpp`

구현 기능:

- Capsule에 부착된 1인칭 Camera
- Enhanced Input Mapping Context 등록
- 이동과 시점 회전
- 점프
- 걷기 `400`, 달리기 `700`

`BP_Exit3Character` Class Defaults에서 다음 에셋이 반드시 연결되어야 한다.

- `DefaultMappingContext` → `IMC_Player`
- `MoveAction` → `IA_Move`
- `LookAction` → `IA_Look`
- `JumpAction` → `IA_Jump`
- `SprintAction` → `IA_Sprint`

### 8.9 `AExit3PlayerController`

파일:

- `Public/Player/Exit3PlayerController.h`
- `Private/Player/Exit3PlayerController.cpp`

UI 생성과 입력 모드 전환을 담당한다.

주요 동작:

- BeginPlay에서 HUD 생성
- GameState의 `OnGameCleared` 구독
- `Esc` 입력으로 Pause Menu 토글
- Pause Menu에서는 `GameAndUI` 입력과 마우스 커서 사용
- Game Clear에서는 `UIOnly` 입력 사용
- 다시 시작 시 현재 Persistent Level 재로딩
- 종료 버튼에서 `QuitGame()` 호출

`BP_Exit3PlayerController`에는 다음 클래스가 지정되어야 한다.

- `HUDWidgetClass` → `WBP_GameHUD`
- `PauseMenuClass` → `WBP_PauseMenu`
- `GameClearWidgetClass` → `WBP_GameClear`

### 8.10 UI 위젯 클래스

#### `UExit3HUDWidget`

GameState의 `OnStageChanged`를 구독하고 다음 형식으로 표시한다.

```text
STAGE 1 / 3
```

UMG 필수 Widget 이름:

- `StageText`

#### `UExit3PauseMenuWidget`

UMG Widget 이름:

- `ResumeButton`
- `QuitButton`

버튼 이벤트는 C++가 자동 연결하므로 Blueprint Event Graph에 같은 동작을 중복 구현하지 않는다.

#### `UExit3GameClearWidget`

UMG Widget 이름:

- `RestartButton`
- `QuitButton`

`BindWidgetOptional`이므로 이름이 틀려도 컴파일 오류는 발생하지 않지만 버튼이 작동하지 않는다. UMG Hierarchy의 Button 이름과 `Is Variable` 설정을 먼저 확인한다.

### 8.11 `UExit3StageData`

Stage와 이상현상 클래스 배열을 담을 수 있는 `UPrimaryDataAsset` 구조다. 현재 런타임의 이상현상 선택은 이 Data Asset을 사용하지 않고 월드의 `AExit3AnomalyActor` 인스턴스를 직접 검색한다.

향후 이상현상을 클래스 기반으로 Spawn하는 구조로 변경할 때 사용할 수 있지만, 현재 코드와 동시에 별도의 후보 목록을 운영하면 설정의 출처가 두 개가 되므로 주의한다.

## 9. 현재 Blueprint 이상현상 구성

현재 에셋:

```text
BP_Anomaly_S1_TestScale
BP_Anomaly_S1_RemoveTriple
BP_Anomaly_S1_BlackSurfaces
```

논리적인 스테이지 배치는 다음과 같다.

| Stage | 이상현상 | 효과 |
| --- | --- | --- |
| Stage 1 | Triple Scale | 세 대상 오브젝트가 정상 Scale의 3배로 커짐 |
| Stage 2 | Remove Triple | 세 대상 오브젝트가 숨겨지고 Collision이 비활성화됨 |
| Stage 3 | Black Surfaces | 중간 Trigger 통과 후 검은 벽·바닥·천장 복사본이 표시됨 |

에셋 폴더와 이름에 `S1`이 남아 있어도 실제 선택 기준은 `SupportedStage` 속성이다. Stage를 변경할 때는 이름만 바꾸지 말고 각 인스턴스의 `SupportedStage`를 확인한다.

### 검은 표면 이상현상의 정상 상태

검은 복사본은 `WallTarget_01`부터 `WallTarget_04`까지 사용한다.

- 레벨에 저장된 기본 상태에서 `Actor Hidden In Game = true`
- Collision은 항상 비활성화
- Anomaly 라운드가 선택되면 Reveal Trigger만 활성화 준비
- 플레이어가 중간 지점을 통과하면 Hidden 해제
- Restore 시 다시 숨김

Normal 라운드에는 `ApplyAnomalyEffect`가 호출되지 않는다. 따라서 정상 상태를 보장해야 하는 값은 Gameplay Level 에셋의 기본값으로도 올바르게 저장되어 있어야 한다.

## 10. C++과 Blueprint 사이의 계약

| 위치 | 계약 |
| --- | --- |
| `BP_Exit3GameMode` | PlayerController는 `BP_Exit3PlayerController`, Pawn은 `BP_Exit3Character` 사용 |
| `L_DefaultMap` | GameMode Override는 `BP_Exit3GameMode` |
| `BP_Exit3Character` | IMC와 네 Input Action 지정 |
| `LevelStreamManager` | Gameplay Level, Side A/B Transform, Blocker, Destination Arrow 정렬 |
| Side Trigger | `ExitSide`와 `ExitDirection` 정확히 지정 |
| GameplayMap Transition Trigger | Source Arrow를 복제 코너의 숨겨지는 기준 위치에 정렬 |
| 이상현상 BP | 고유 `AnomalyId`, 올바른 `SupportedStage`, `bEnabled = true` |
| `WBP_GameHUD` | `StageText` 존재 |
| `WBP_PauseMenu` | `ResumeButton`, `QuitButton` 존재 |
| `WBP_GameClear` | `RestartButton`, `QuitButton` 존재 |

## 11. 반드시 유지해야 하는 불변 조건

1. `StageManager`와 `LevelStreamManager`는 Persistent Level에 각각 하나만 둔다.
2. 새 Gameplay Level의 이상현상은 `OnLevelShown` 이후 검색한다.
3. Gameplay Level 준비 전에는 `bDecisionLocked`를 해제하지 않는다.
4. Side A와 Side B는 각각 전용 Gameplay Transform을 사용한다.
5. 모든 이상현상 ID는 전체 월드에서 고유해야 한다.
6. 복원이 필요한 이상현상 대상은 파괴하지 않는다.
7. Normal 라운드에서 보여야 할 상태는 레벨의 기본 저장 상태와 일치해야 한다.
8. 순간이동 Source와 Destination의 Forward 방향과 높이를 일치시킨다.
9. 핵심 판정 로직을 Level Blueprint에 복제하지 않는다.
10. UMG의 C++ 연결 이름은 대소문자까지 동일하게 유지한다.

## 12. 새 이상현상 추가 방법

1. `AExit3AnomalyActor`를 부모로 하는 Blueprint를 생성한다.
2. 고유한 `AnomalyId`를 지정한다.
3. `SupportedStage`를 지정한다.
4. Gameplay Level에 인스턴스 하나를 배치한다.
5. 대상 Actor 참조를 Instance Editable 배열로 연결한다.
6. `CaptureNormalState`에서 복원에 필요한 값을 저장한다.
7. `ApplyAnomalyEffect`에서 한 번의 이상 효과를 적용한다.
8. `RestoreNormalState`에서 저장된 값으로 되돌린다.
9. Normal 라운드, Anomaly 라운드, 다음 라운드 복원까지 연속 테스트한다.

후보가 둘 이상인 Stage에서는 `StageManager`가 직전에 선택된 `AnomalyId`를 한 번 제외한다. 후보가 하나라면 그대로 다시 선택한다.

## 13. 로그를 이용한 문제 진단

### 후보 수가 0일 때

```text
No valid anomaly candidates were found for Stage 1.
```

확인 순서:

1. 이상현상 BP 인스턴스가 `L_GameplayMap`에 있는가
2. BP 부모가 `Exit3AnomalyActor`인가
3. `AnomalyId`가 None이 아닌가
4. `SupportedStage`가 현재 Stage와 같은가
5. `bEnabled`가 true인가
6. 다른 인스턴스와 ID가 중복되지 않는가
7. `Anomaly candidates refreshed: Total=N` 로그가 0인지 확인

### Gameplay Level이 열리지 않을 때

```text
Gameplay level recreation failed: no GameplayLevel is configured
Gameplay level load request failed
```

`LevelStreamManager`의 `GameplayLevel` Soft Reference와 Side별 Transform을 확인한다.

### 통로 판정이 발생하지 않을 때

- Trigger의 `ExitDirection` Arrow 확인
- 캐릭터가 Arrow의 Forward 방향으로 움직이는지 확인
- `MinimumExitDirectionDot`가 지나치게 높지 않은지 확인
- 현재 `bDecisionLocked` 상태인지 로그 확인

### 순간이동이 실패할 때

```text
Passage transition failed: destination A/B is blocked.
```

Destination Arrow가 벽이나 Blocker 내부에 있지 않은지 확인하고, 캐릭터 Capsule이 들어갈 공간을 확보한다.

### UI가 보이지 않거나 버튼이 작동하지 않을 때

- `BP_Exit3GameMode`의 Player Controller Class 확인
- `BP_Exit3PlayerController`의 세 Widget Class 확인
- UMG Widget 이름 확인
- 새 C++ UCLASS 추가 후 에디터를 완전히 재시작했는지 확인

## 14. 빌드 방법

프로젝트 루트에서 다음 명령으로 Editor Target을 검증한다.

```powershell
& 'C:\Program Files\Epic Games\UE_5.6\Engine\Build\BatchFiles\Build.bat' Exit3Editor Win64 Development `
  -Project='C:\UnrealProject\CH3-3-Exit3\Exit3\Exit3.uproject' `
  -WaitMutex `
  -NoHotReloadFromIDE
```

새 C++ 클래스나 `UPROPERTY`를 추가한 경우:

1. Unreal Editor 종료
2. 위 명령으로 빌드
3. Editor 재실행
4. Blueprint 부모와 Class Defaults 재확인

Editor가 DLL을 사용 중이면 컴파일은 완료되어도 Link 단계가 실패할 수 있다.

## 15. 통합 테스트 체크리스트

- [ ] 게임 시작 시 Stage 1 HUD가 표시된다.
- [ ] 최초 Gameplay Level이 Side A에 생성된다.
- [ ] 로딩 중 양쪽 입구가 차단된다.
- [ ] Normal 라운드에는 이상현상이 보이지 않는다.
- [ ] 각 Stage의 Anomaly 라운드에는 해당 이상현상 하나만 적용된다.
- [ ] 되돌아오면 Anomaly 판단으로 처리된다.
- [ ] 끝까지 진행하면 반대편 통로로 자연스럽게 이동하고 Normal 판단으로 처리된다.
- [ ] 정답 시 다음 Stage로 진행한다.
- [ ] 오답 시 Stage 1로 초기화한다.
- [ ] 이전 라운드의 이상 상태가 남지 않는다.
- [ ] Stage 3 정답 후 Game Clear 화면이 표시된다.
- [ ] 다시 시작하면 플레이어 위치와 모든 상태가 초기화된다.
- [ ] Esc와 계속하기 버튼으로 Pause 상태가 정상 전환된다.
- [ ] 패키징된 빌드에서 종료 버튼이 동작한다.

## 16. 현재 구조의 확장 지점

- `OnRoundStarted`: Stage별 환경음과 조도 변경
- `OnDecisionResolved`: 짧은 성공·실패 연출
- `OnAnomalyActivated`: 디버그 표시나 이상현상별 사운드 재생
- `UExit3StageData`: 향후 Data Asset 기반 후보 관리
- UI Widget Class: Main Menu 또는 설정 화면 추가

확장 기능은 기존 GameMode의 판정 순서를 변경하기보다 공개 Delegate를 구독하는 방식으로 연결하는 것이 안전하다.

