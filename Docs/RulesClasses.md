# 기능별 규칙 클래스 분리

## 계획과 완료 기준

1. 기존 Rules.cpp별로 독립 C++ 클래스와 같은 이름의 헤더를 만든다.
2. 실제 로직은 기능 클래스가 소유하고 기능 사이의 호출도 해당 클래스를 직접 사용한다.
3. UTDGameplayLibrary는 기존 Blueprint 호출을 전달하는 호환 입구로만 유지한다.
4. 사용자 정의 구조체를 받는 세 함수는 Blueprint 인수 해석만 입구에 남기고 자료 처리를 데이터 클래스로 옮긴다.
5. 함수 계산식·자산·이벤트 순서를 보존하고 Editor·게임 빌드, 기존 93개 검사와 88개 맵 검사를 다시 실행한다.

기능 클래스는 상태를 별도로 저장하지 않는 일반 C++ 클래스다. 기존 Blueprint 변수의 저장 위치와 부모 클래스는 바꾸지 않는다.
이번 요청은 책임 분리이며 성능 최적화, 자료형 이전, 자산 재저장은 포함하지 않는다.

## 결과

계획 1~5를 완료했다. 13개 클래스가 각각 같은 이름의 헤더와 구현 파일을 갖는다.

| 클래스 | 책임 |
| --- | --- |
| FTDPlayerRules | 선택·설치·합성·판매 입력 |
| FTDCameraRules | 카메라 이동·확대 |
| FTDMonsterRules | 몬스터 활성화·피해·보상 |
| FTDAIRules | AI 제어·경유지 선택 |
| FTDPortalRules | 몬스터 생성·재사용·생성 타이머 |
| FTDTowerRules | 대상 진입·이탈·공격 신호·자리 반환 |
| FTDTowerAttackRules | 타워 종류별 공격 |
| FTDProjectileRules | 발사체 충돌·활성화 |
| FTDHealthRules | 체력·사망 알림 |
| FTDEconomyRules | 골드 지출·획득 |
| FTDRoundRules | 라운드·남은 시간·몬스터 수 |
| FTDGridRules | 설치 격자 생성 |
| FTDDataRules | 기존 구조체 자료 조회·초기화 |

호출 흐름: 기존 Blueprint → UTDGameplayLibrary → 해당 FTD*Rules 클래스.
클래스 사이에서는 호환 입구를 거치지 않는다. 예를 들어 FTDMonsterRules는
FTDHealthRules, FTDEconomyRules, FTDRoundRules를 직접 호출한다.
공통 입구의 54개 일반 함수는 전달만 수행하며, 구조체 함수 3개는 Blueprint 인수를
해석한 뒤 FTDDataRules에 처리를 맡긴다. 기능 클래스는 공통 입구를 포함하거나 상속하지 않는다.
별도 객체 생성이나 Tick 추가는 없고, 클래스의 static 함수가 전달받은 기존 객체의 상태를 처리한다.

## 검증

현재 Windows 11·UE 5.8.2 호스트에서 실제 실행한 결과다.

- 통과: Editor 및 게임 Win64 Development 빌드. MSVC 14.44.35229,
  Windows SDK 10.0.22621.0에서 클래스 선언·정의·모듈 연결을 확인했다.
- 통과: 일반 함수 54개의 구현을 이전과 비교해 클래스 소유권·직접 호출 경로·포함 헤더
  외의 변경이 없음을 확인했다. 구조체 함수 3개도 기존 자료 처리와 인수 해석 순서를 보존했다.
- 통과: TDVerify 93개, 실패 0개. 자산을 저장하지 않고 기존 호출 연결의 결과를 검사했다.
  플러그인 EULA 안내 경고는 검사 실패가 아니다.
- 통과: MainLevel 화면 없는 통합 검사 88개, 실패 0개. 설치·합성·판매,
  몬스터 피해·보상·재사용, 라운드 변경을 검사했다. 69개는 각 몬스터의 라운드 값 확인이다.
  기존 시험과 동일하게 실행 인수에서만 에디터용 Python 관련 플러그인을 껐다.
- 통과: Visual Studio 프로젝트 재생성과 새 Rules.h 13개 등록 확인.
  .NET 10 자동화 프로젝트의 VS 2026 안내는 게임 솔루션 생성 실패가 아니다.
- 미검증: 실제 화면·소리·마우스 조작, 전체 10라운드 플레이, 배포본 실행,
  Windows 이외 환경 및 성능 개선. 화면 없는 Windows 검사는 이 범위를 증명하지 않는다.

로그는 Saved/Logs/RulesClassesVerify.log와 RulesClassesIntegration.log에 있다.
Content 자산, 부모 클래스, 기존 게임 계산식은 변경하지 않았다.

## 지식 기록 검토

범위는 current-project이며 재사용할 사실은 기능 클래스가 로직을 소유하고
UTDGameplayLibrary는 기존 Blueprint를 위한 전달 입구로 유지한다는 구조다.
현재 Windows 호스트에서 Hive 조회와 지식 검사를 실제 실행했으나 기존 Wiki의 출처 누락과
지식 통합 잠금 시간 초과가 확인됐다. 별도 정본·색인 등록은 완료하지 않았으며 성공으로
간주하지 않는다. 기존 지식과 잠금은 수정하지 않았고 이번 구조의 기준은 이 문서다.
