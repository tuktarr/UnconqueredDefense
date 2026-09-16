# 블루프린트 로직 C++ 이전

## 완료 기준

원본 그래프의 실행 연결, 입력 기본값, 자료형과 컴포넌트 설정을 대조한다.
C++ 빌드, 변경 블루프린트 컴파일, 실제 게임 시험을 각각 확인한다.
기존 C++ 초안은 아직 원본 동작과 일치한다고 확인되지 않았다.

## 실행 순서

1. 현재 빌드 오류를 고치고 에디터 모듈을 빌드한다.
2. 읽기 전용 검사 명령으로 전체 그래프 연결과 기본값을 내보낸다.
3. 원본과 다른 C++ 초안을 수정하고 게임 규칙을 의존 순서대로 옮긴다.
4. 기존 자산의 참조와 표시 설정을 보존하면서 C++ 호출을 연결한다.
5. 변경 자산 전체 컴파일과 게임 동작 시험을 수행한다.

## 적용 내용

기존 자산 19개의 로직 진입점 57개를 `UTDGameplayLibrary` 호출로 연결했다.
목록은 `Scripts/BlueprintMigration/routes.json`에 있다.
이전 코드는 골드·라운드·무작위 타워 선택, 체력·사망 보상, 몬스터 재사용과 생성,
AI 경로 선택, 카메라 이동·확대, 격자 생성, 타워 설치·선택·합성·판매,
공격 신호·다섯 종류의 공격·투사체 충돌·보스 등장 처리를 포함한다.

실제 게임은 기존 Blueprint 클래스를 계속 사용하되 게임 규칙을 C++에 맡긴다.
구현은 `Source/TowerDefense`의 기능별 폴더에 있는 `*Rules.cpp`로 나누었다.
각 기능의 선언은 해당 `*Rules.h`에 있고 구현은 독립된 `FTD*Rules` 클래스가 소유한다.
`BlueprintBridge/TDGameplayLibrary.h/.cpp`는 기존 Blueprint 호출을 전달하는 호환 입구다.
폴더 안내는 `Docs/SourceStructure.md`, 클래스 분리 안내는 `Docs/RulesClasses.md`에 있다.
이전의 `ATDGameMode`, `ATDTowerBase` 등 별도 클래스 초안으로
자산의 부모 클래스를 바꾸지 않았다. 그 초안들은 이번 이전의 실행 경로가 아니다.

기존 변수·열거형·구조체·데이터 표는 저장 형식과 자산 참조를 깨뜨리지 않도록 유지한다.
`TDLegacyAccess.h`와 `TDLegacyData.h`가 그 값만 읽고 쓰는 연결 부분이다.
화면 구성, 재질·애니메이션, 입력 이벤트 입구, 컴포넌트 구성, 초기 표시 설정과
이벤트 연결은 Blueprint에 남긴다. 몬스터 사망 애니메이션과 1초 지연도 유지한다.
따라서 Blueprint 자산을 없애거나 모든 자료형을 네이티브 클래스로 바꾼 작업은 아니다.

원래 계산 조건은 임의로 개선하지 않았다. 라운드 시간 30초, 음수 비용 처리,
몬스터 수 감소의 하한 없음, 풀 크기보다 1개 적게 준비하는 조건, 최상위 등급 판매 금지,
애니메이션 신호에 따른 공격 시점이 이에 해당한다.

## 검증 기록 (Windows 11, Unreal Engine 5.8.2)

- 통과: Build Tools 2022 / MSVC 14.44.35228 / Windows SDK 10.0.26100.0으로
  Editor C++ 빌드. 이전 지역 변수 이름 충돌도 수정했다.
- 통과: 같은 Windows 호스트에서 에디터가 아닌 `TowerDefense Win64 Development`도
  실제 빌드했고 `Binaries/Win64/TowerDefense.exe`를 생성했다. 자산을 묶은 배포본의
  생성·설치·실행 시험은 이 C++ 이전 작업에 포함하지 않았다.
- 통과: 이전 자산뿐 아니라 화면·애니메이션·자식 자산을 포함한
  `/Game/Blueprint` 아래 Blueprint 46개를 실제 컴파일했다. 컴파일 오류 0개.
  플러그인의 UE EULA 안내 경고 1개는 이번 코드의 컴파일 오류가 아니다.
- 통과: `TDVerify`의 자동 확인 93개. 원본과 골드·라운드 표·초기 체력을 비교했고,
  추가 이전 전에 무작위 선택 50가지와 등급 변화 5가지를 원본과 비교했다.
  피해·중복 사망 방지·입력 상태·설치 비용도 시험했다.
  저장 후 다시 불러온 자산으로 같은 시험을 재실행했다. 저장 후 비교는 원본 독립 비교가
  아니라 연결된 함수와 C++의 회귀 검사다.
- 통과: 실제 `/Game/Maps/MainLevel`에서 화면 없는 통합 시험 88개.
  준비된 몬스터 69개, 네 경유지, 타워 설치·합성·판매·자리 반환,
  피해·보상·중복 보상 방지, 라운드 변경·재생성을 확인했다.
  88개 중 69개는 각 몬스터의 라운드 값 확인이다.
- 한계: 화면 없는 실행이므로 실제 마우스 위치, 화면 배치·효과·소리는 검증하지 않았다.
  모든 타워의 발사체 궤적과 전체 10라운드 승패를 직접 플레이한 결과도 아니다.
- 배포 자산 누락 예방: C++ 경로로 불러오는 `/Game/Blueprint`를 패키징 포함 대상으로
  지정한다. 이 설정만으로 완성된 배포 파일의 실행까지 증명하지는 않는다.

처음 일반 게임 실행에서는 UE 편집기용 Python 플러그인이 게임 모드에서 찾을 수 없는
`AgentSkill` / `PythonTestRunner` 오류를 냈다. 통합 시험에서는 실행 인수로만
EditorToolset, ToolsetRegistry, ModelContextProtocol, PythonScriptPlugin을 껐으며
프로젝트의 플러그인 사용 설정은 변경하지 않았다. 이후 통합 시험 로그에는 오류가 없었다.

검증 로그는 `Saved/Logs/MigrationVerifyOriginal.log`, `MigrationVerifyExpanded.log`,
`MigrationVerifyFinal.log`, `MigrationApply2.log`, `MigrationIntegration.log`에 있다.
로그와 `Saved`는 버전 관리 대상이 아닌 재생성 가능한 자료다.

## 재실행과 복구

- C++ 빌드 후 `UnrealEditor-Cmd.exe TowerDefense.uproject -run=TDVerify -unattended -nullrhi -nosound`.
- 실제 맵 시험: 프로젝트 뒤 `/Game/Maps/MainLevel -game -TDGameplaySmoke -unattended
  -nullrhi -nosound -benchmark -seconds=180 -fps=30
  -DisablePlugins=EditorToolset,ToolsetRegistry,ModelContextProtocol,PythonScriptPlugin`.
  `TDGameplaySmoke`가 있을 때만 시험 상태를 만들며 일반 플레이와 Shipping에서는 실행되지 않는다.
- 연결 검사: `-run=TDMigrate -Manifest=<routes.json 절대 경로>`는 저장하지 않는다.
  `-Apply`를 추가하면 모두 컴파일한 뒤 바뀐 자산을 저장한다. 재실행은 중복 호출을 만들지 않는다.
- 자산을 처음 변경하기 전 파일은 `Saved/BlueprintMigration/Originals`에 19개 보관했다.
  원본 노드도 자산 안에 실행선이 끊긴 상태로 보관한다. 새 C++ 호출과 동시에 실행되지 않는다.
  복구가 필요하면 에디터를 닫고 필요한 원본만 기존 Content 경로에 복사한다.
  백업의 파일 이름과 Content 경로는 `routes.json`으로 대응시킬 수 있다.

## 프로젝트 지식 기록 상태

기억 후보의 범위는 `current-project`이며, 재사용할 사실은 기존 Blueprint 저장 형식을
유지하면서 `UTDGameplayLibrary`에 게임 규칙을 맡긴다는 구조다.
별도 Hive 기록은 기존 `blueprint-cpp-migration-plan.md`의 출처 누락과
지식 통합 잠금 시간 초과 때문에 검증에 실패했다. 기존 지식이나 잠금은 삭제하지 않았다.
이번 구현의 기준 문서는 이 파일이며, 오래된 Wiki의 SDK 미설치·부모 클래스 변경 계획은
현재 구현 상태를 나타내지 않는다.
