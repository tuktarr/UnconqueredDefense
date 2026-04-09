## UnconqueredDefense Game

#### 프로젝트 개요
- 장르 :  트랙루프형 디펜스게임
- 개발 기간 : 3/24 - 4/7 (14일)
- 핵심 시스템
  => 승패 조건
  ==> 승리 조건 : 10라운드 까지의 몬스터 웨이브를 버텨내기
  ==> 패배 조건 : 보스라운드(5, 10라운드)를 라운드 시간내에 클리어 하지 못하거나 몬스터 개체 수가 필드에 60마리 이상 쌓이면 패배
  => 트랙 루프 로직
  ==> 몬스터는 Portal에서 소환되며 CheckPoint 1 -> 2 -> 3 -> 4 -> 1 이런식으로 트랙을 순회함
  => 전술 자원 시스템
  ==> Gold : 적군 처치 시, 혹은 Supporter 타워 
- 참여 인원 : 1인
- 결과물 링크 : <https://blog.naver.com/hj10105/224212907806>
- 회고록 : <https://blog.naver.com/tuktarr/224244809465>

#### 기술스택
- Language : C++
- Environment : Unreal Blueprint
- Version Control : Git / GitHub
- AI : Gemini

#### 핵심 시스템 설계
1. 프로그래밍 언어 및 설계 (C++ & OOP)
- 클래스 상속 구조 (Class Hierarchy):
 => BP_TowerBase: 모든 타워의 부모 클래스입니다. 공격 범위, 공격 속도, 업그레이드 비용 등의 변수를 선언하고, 타겟팅 로직을 공통으로 처리합니다.
 => BP_Monster: 모든 몬스터의 부모 클래스입니다. 체력, 골드 보상 등을 관리합니다.
- 컴포넌트 기반 설계:
  => HealthComponent나 ProjectileMovement같이 각자의 역할을 할 수 있도록 캐릭터에 컴포넌트를 부착해서 컴포넌트 내부적으로 처리함
- 블루프린트 인터페이스 (BPI) + EventDispatcher:
  => 클래스 간 의존성(Coupling)을 낮춥니다
2. 게임 시스템 구현 (Game Systems)
  - 그리드 및 배치 시스템 (Grid & Placement):
  => Line Trace: 마우스 커서 위치에서 바닥으로 레이저를 쏴 TowerSlot에서 배치 가능 여부를 확인합니다.
  => 9X8 의 TowerSlot을 미리 생성해두고 그 위에 타워를 건설
  - Snap to Grid: Vector / GridSize 후 다시 * GridSize 연산을 통해 타워가 정확한 칸에 배치되도록 구현합니다.
  => TowerSlot의 범위 밖이면 아예 건설 안되도록 설정
  - 웨이브 관리 시스템 (Wave Manager):
  => DataTable : 웨이브데이터(CurrentRoundCount)를 이용해서 몬스터의 ABP, SkeletalMesh 설정 후, 스포닝풀에서 소환
  ==> 몬스터의 데이터에 맞게 EventDispatcher를 이용해서 처치할 시, 플레이어의 Gold에 RewardGold를 합니다. 
  => Timer by Event: 일정한 간격으로 적을 생성하는 루프를 돌립니다. 타이머 핸들로 스폰을 종료합니다.
3. 최적화 및 연출 (Optimization & UX)
  - 오브젝트 풀링 (Object Pooling):
  => 수많은 투사체(Projectile)와 적(Enemy)을 매번 생성/삭제(Spawn/Destroy)하지 않고, 미리 생성해 둔 뒤 활성화/비활성화하여 CPU 부하를 줄입니다.
  - 시각적 피드백(UX)
  => Widget Component: 적의 머리 위에 HP 바를 띄우고, 타워 선택 시 공격 범위를 보여주는 시각적 가이드를 구현합니다.
