# 타워 유휴 Tick 최적화

## 목적

공격 타워가 대상이 없을 때 Blueprint `Tick`을 멈추고, 대상이 생기면 다시 켠다.
총기 타워를 기준으로 변경 전과 변경 후를 Unreal Insights에서 측정한 뒤 같은 규칙을 다섯 타워에 적용한다.

## 계획

1. `BP_Tower_Gun`의 시작 상태에서 Actor Tick을 끈다.
2. 첫 대상이 사거리에 들어오면 Tick을 켜고, 마지막 대상이 나가면 Tick을 끈다.
3. 자동 검증에 시작 상태와 대상 진입·이탈 상태 전환을 추가한다.
4. Blueprint를 컴파일하고 C++ 모듈을 빌드한 뒤 기존 검증을 실행한다.
5. 변경 전과 같은 다수 타워 부하 장면을 Unreal Insights로 기록한다.
6. `BP_Tower_Gun_C`와 전체 프레임 시간을 비교하고 결과 화면을 저장한다.

## 성공 기준

- 타겟이 없는 총기 타워의 Actor Tick이 꺼진다.
- 첫 타겟 진입 시 Tick이 켜지고 마지막 타겟 이탈 시 다시 꺼진다.
- 기존 공격·타겟 배열 동작을 깨뜨리지 않는다.
- Unreal Insights 화면에서 변경 후 측정 구간과 시간을 읽을 수 있다.
- 이전 측정과 같은 계산 방식으로 성능 차이를 제시한다.

## 결과

### 구현

- `BP_Tower_Gun`의 `PrimaryActorTick.bStartWithTickEnabled`를 `False`로 바꿨다.
- 첫 몬스터가 `TargetArray`에 들어오면 총기 타워 Tick을 켠다.
- 마지막 몬스터가 나가거나 공격 종료 때 대상 배열이 비면 Tick을 끈다.
- 초기 측정 단계에서는 총기 타워에만 적용했고, 이후 전체 적용 단계에서 다섯 타워로 확장했다.

### 검증

- Windows 11, Unreal Engine 5.8.2의 `TowerDefenseEditor Win64 Development` 정식 빌드가 통과했다.
- `TDVerify`의 프로젝트 규칙 확인은 `96개 통과, 0개 실패`였다.
- 새 확인 3개는 시작 시 Tick 꺼짐, 첫 대상 진입 시 켜짐, 마지막 대상 이탈 시 꺼짐이다.
- 명령let 프로세스 종료값은 기존 `GameFeatureData` 자산 관리자 설정 오류 2개 때문에 `1`이었다.
  새 Tick 확인 자체는 모두 통과했지만, 이 결과는 프로젝트 전체 명령let 실행이 오류 없이 끝났다는 뜻은 아니다.

### Unreal Insights 측정

- 변경 전 원자료: `Saved/Profiling/20261007_201600_231860.utrace`
- 변경 후 원자료: `Saved/Profiling/Insights/TowerTickOptimization/After_NoTargetFar_26Towers.utrace`
- 조건: 총기 타워 26개, 게임 경로에서 멀리 배치, 대상 없음, CPU/Frame 추적
- 변경 전 `ExecuteUbergraph_BP_Tower_Gun`: `82,920회`, 총 포함 시간 `3.951411초`,
  엔진 프레임 `3,174개` 기준 `26.12회/프레임`, `1.245 ms/프레임`
- 변경 후 `ExecuteUbergraph_BP_Tower_Gun`: 통계에서 사라짐, `0회`, `0 ms`
- 변경 후 남은 `BP_Tower_Gun_C` 타이머 `131회`는 생성·시작 같은 생명주기 호출이며,
  프레임마다 반복되는 Blueprint Tick 함수는 아니다.

이 비교는 대상 없는 총기 타워의 Blueprint Tick 경로가 제거됐음을 증명한다. 두 추적의 전체 장면과
수집 길이가 완전히 같지는 않으므로, 게임 전체 프레임 시간이 정확히 `1.245 ms` 줄었다고 단정하지 않는다.

### 증거 파일

- `Saved/Profiling/Insights/TowerTickOptimization/AfterFar_TimerStatistics.csv`
- `Saved/Profiling/Insights/TowerTickOptimization/AfterFar_Export.log`
- `Saved/Profiling/Insights/TowerTickOptimization/Insights_Comparison.png`
- `Saved/Profiling/Insights/TowerTickOptimization/Insights_Comparison.html`

현재 실행 환경은 별도 Windows 앱 창 캡처를 제공하지 않아 Unreal Insights 창 자체의 픽셀 화면은
저장하지 못했다. 대신 Unreal Insights가 직접 내보낸 타이머 CSV로 같은 수치를 그린 PNG를 저장했다.

## 전체 타워 적용 계획

1. 다섯 타워 Blueprint의 `PrimaryActorTick.bStartWithTickEnabled` 실제 기본값을 검사한다.
2. 시작 Tick이 켜진 타워는 대상이 없는 상태에서 Tick이 실행되지 않도록 기본값을 끈다.
3. 공통 대상 진입·이탈 규칙이 총기 타워에만 한정되지 않고 다섯 타워에 적용되도록 수정한다.
4. 다섯 타워 각각에 대해 시작 시 꺼짐, 첫 대상 진입 시 켜짐, 마지막 대상 이탈 시 꺼짐을 자동 검증한다.
5. C++ 빌드와 `TDVerifyCommandlet`를 실행해 기존 전투 규칙이 유지되는지 확인한다.

## 전체 타워 적용 진행 상태

- 적용 전에는 `BP_Tower_Gun`만 시작 Tick이 꺼져 있었고, `BP_Tower_Sword`, `BP_Tower_Robot`,
  `BP_Tower_Magic`, `BP_Tower_Support`는 시작 Tick이 켜져 있었다.
- 네 타워 Blueprint의 `PrimaryActorTick.bStartWithTickEnabled`를 `False`로 변경하고 저장했다.
- 독립 자산 검사에서 다섯 타워 모두 시작 Tick이 꺼진 상태로 확인됐다.
- 공통 대상 진입·이탈 규칙의 총기 타워 전용 제한을 제거해 모든 타워에 같은 Tick 전환을 적용했다.
- 자동 검증은 다섯 타워마다 시작 시 꺼짐, 첫 대상 진입 시 켜짐, 마지막 대상 이탈 시 꺼짐을 확인하도록 확장했다.
- Windows 11과 Unreal Engine 5.8.2에서 `TowerDefenseEditor Win64 Development` 빌드를 실제 실행했고 성공했다.
- `TDVerifyCommandlet`의 전체 검사 결과는 `119개 통과, 0개 실패`였다. 이 중 15개는 다섯 타워의
  시작 시 Tick 꺼짐, 첫 대상 진입 시 켜짐, 마지막 대상 이탈 시 꺼짐을 각각 확인한다.
- 명령let 자체는 결과 `0`으로 끝났지만 프로세스 종료 코드는 `1`이었다. 원인은 이번 변경과 무관한 기존
  `GameFeatureData` 자산 관리자 설정 오류와 쓰기 가능한 Derived Data Cache가 없다는 환경 오류다.
  따라서 타워 규칙과 기존 자동 검사 통과는 증명하지만, 프로젝트의 모든 Unreal 시작 오류가 해결됐다는 뜻은 아니다.
