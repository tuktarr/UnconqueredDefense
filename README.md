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

2. 게임 시스템 구현 (Game Systems)

3. 최적화 및 연출 (Optimization & UX)
