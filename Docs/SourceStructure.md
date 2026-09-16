# C++ 소스 폴더 구조

## 정리 계획

1. 게임 모듈의 Public/Private 구분을 없애고 기능별 폴더에 헤더(.h)와 구현(.cpp)을 함께 둔다.
2. Character 아래 Player, Monster, Tower를 두고, GameMode는 Economy와 Rounds로 세분화한다.
3. 여러 기능이 섞인 이전 코드는 함수 내용과 Blueprint 호출 이름을 유지한 채 해당 기능 폴더로 나눈다.
4. 에디터 전용 이전 도구는 게임 모듈과 분리하되 Commandlets와 Tests로 정리한다.
5. 포함 경로와 Visual Studio 프로젝트 파일을 갱신하고 Editor·게임 빌드 및 기존 시험을 실행한다.

## 지킬 범위

폴더는 코드를 찾기 위한 분류다. Character 폴더에 둔 Tower를 Unreal의 ACharacter로 바꾸지는 않는다.
모듈 이름, C++ 클래스 이름, Blueprint 호출 이름, Content 자산과 게임 동작은 변경하지 않는다.
기존 클래스 초안과 현재 연결된 UTDGameplayLibrary 구현도 임의로 삭제하지 않는다.

## 모듈의 역할

- TowerDefense: 실제 게임 코드. 이 폴더를 중심으로 게임을 수정한다.
- TowerDefenseMigration: Blueprint 검사·C++ 연결·자동 검증에 쓰는 에디터 전용 도구.
  게임 배포 대상에 포함되지 않도록 별도 모듈로 유지한다.

Public/Private 폴더는 C++의 public/private 접근 지정자와 다르다.
기존에는 외부 모듈이 쓰는 헤더와 내부 구현을 나누는 Unreal 관례를 따른 것이다.
이번 구조에서는 모듈의 Build.cs에 포함 경로를 명시하고 기능별로 같은 폴더에 둔다.

## 적용한 구조

정리 계획의 1~5단계를 완료했다. 헤더와 구현을 같은 기능 폴더에 둔다.

```text
Source/
├─ TowerDefense/                 실제 게임
│  ├─ Character/
│  │  ├─ Player/                 플레이어·카메라
│  │  ├─ Monster/
│  │  │  ├─ AI/                  이동 판단
│  │  │  └─ Spawning/            생성·재사용
│  │  └─ Tower/
│  │     ├─ Combat/              공격
│  │     └─ Projectile/          발사체
│  ├─ GameMode/
│  │  ├─ Economy/                골드·비용
│  │  └─ Rounds/                 라운드
│  ├─ Components/Health/         체력
│  ├─ World/Grid/                격자·설치 자리
│  ├─ Data/                      자료형·데이터 표
│  ├─ BlueprintBridge/           기존 Blueprint와 연결
│  └─ Tests/                     명시적으로 켤 때만 실행하는 시험
└─ TowerDefenseMigration/        에디터 전용 이전 도구
   ├─ Commandlets/
   │  ├─ Inspect/                원본 검사
   │  └─ Migrate/                C++ 호출 연결
   └─ Tests/                     연결 결과 검증
```

현재 Blueprint에서 실행하는 게임 규칙은 기능별 `*Rules.cpp`에 있다.
각 `*Rules.h/.cpp`는 독립된 `FTD*Rules` 클래스의 선언과 구현이다.
`BlueprintBridge/TDGameplayLibrary.h/.cpp`는 기존 Blueprint 호출을 기능 클래스에 전달하는 호환 입구다.
기능별 클래스는 이 입구를 참조하지 않으며 서로 필요한 클래스를 직접 호출한다.
자세한 분리 범위는 `Docs/RulesClasses.md`에 있다.
기존 `ATDGameMode`, `ATDTowerBase` 등의 클래스 초안은 컴파일되지만 현재 자산의 부모가 아니다.
폴더 정리만으로 이 클래스들에 실행 경로를 바꾸지 않았다.

## 검증 (2026-09-16)

현재 Windows 11 호스트와 Unreal Engine 5.8.2에서 아래 항목을 실제 실행했다.

- 통과: Editor와 게임 Win64 Development 빌드. VS Community 2022의 MSVC 14.44.35229,
  Windows SDK 10.0.22621.0을 사용했다. 모듈의 컴파일·연결을 확인한 것이며 배포본 시험은 아니다.
- 통과: Visual Studio 프로젝트 파일 재생성. 새 기능별 경로가 프로젝트 목록에 포함되고
  옛 Public/Private 파일 경로가 남지 않았음을 확인했다. 기존 `.vsconfig` 내용도 유지했다.
  UE의 .NET 10 자동화 프로젝트는 VS 2026용이라는 안내가 있었으나 게임 솔루션 생성은 성공했다.
- 통과: 기존 57개 `UTDGameplayLibrary` 함수 본문이 이동 전과 동일하다.
  이것만으로 모든 실행 동작을 증명하지는 않으므로 다음 두 시험도 수행했다.
- 통과: `TDVerify` 93개, 실패 0개. 자산을 저장하지 않고 연결된 게임 규칙을 검사했다.
  플러그인 EULA 안내 경고 1개는 검사 실패가 아니다.
- 통과: 실제 MainLevel의 화면 없는 통합 시험 88개, 실패 0개.
  타워 설치·합성·판매, 몬스터 피해·재사용, 라운드 변경을 확인했다.
  88개 중 69개는 각 몬스터의 라운드 값 확인이다.
- 미검증: 화면·소리·실제 마우스 조작과 전체 10라운드 플레이, 패키징한 배포본,
  Windows 이외 운영체제. 이번 실행은 화면 없는 Windows 시험이므로 이 범위는 증명하지 않는다.

로그: `Saved/Logs/SourceStructureVerify.log`, `Saved/Logs/SourceStructureIntegration.log`.
통합 시험은 이전과 동일하게 실행 인수에서만 에디터용 Python 관련 플러그인을 껐다.
Content 자산, Blueprint 호출 이름, 게임 계산식은 이번 정리에서 바꾸지 않았다.
비어 있던 Public/Private 폴더 네 개만 제거했으며 모든 코드는 새 기능 폴더에 보존했다.

## 지식 기록 검토

범위는 `current-project`다. 재사용할 결정은 C++ 소스를 Public/Private 대신 기능별로
분류하고 헤더와 구현을 함께 두되 에디터 전용 이전 도구는 별도 모듈로 유지한다는 것이다.
Hive 조회는 기존 지식 통합 잠금 시간 초과로 실패했다. 별도 지식 등록도 필요한 입력 형식을
확보하지 못해 완료하지 않았으며 정본·색인 등록 성공으로 간주하지 않는다.
기존 지식과 잠금은 수정하지 않았다. 이번 구조의 기준 문서는 이 파일이다.
