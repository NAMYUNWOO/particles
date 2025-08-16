### context engineering 
```
  주요 Context Engineering 구조:

  1. 계층적 컨텍스트 시스템

  CLAUDE.md (최상위 지시문)
  ├── PLANNING.md (아키텍처 가이드)
  ├── TASK.md (작업 추적)
  ├── RAYLIB_CONVENTIONS.md (코딩 규칙)
  └── PRPs/ (상세 구현 프롬프트)

  2. 핵심 파일들의 역할

  CLAUDE.md - AI 행동 계약서
  - 프로젝트 인식 규칙 (PLANNING.md 필수 읽기)
  - 코드 품질 기준 (500줄 제한, 모듈화)
  - 필수 테스트 작성
  - 문서 업데이트 의무화

  PRPs/ - Product Requirements Prompts
  - 완전한 구현 컨텍스트를 하나의 문서로 제공
  - 검증 게이트와 안티패턴 포함
  - 예: hp-potion-item.md는 9/10 신뢰도로 전체 기능 구현

  3. Context Engineering 기법들

  참조 기반 학습
  - 기존 코드를 패턴으로 활용
  - examples/ 디렉토리를 통한 패턴 매칭

  검증 중심 개발
  - 빌드 명령어와 테스트 포함
  - 메모리 누수 탐지 전략

  점진적 공개
  - 상위 규칙 → 세부 구현으로 진행
  - 필요시에만 깊은 컨텍스트 제공

  안티패턴 문서화
  - 하지 말아야 할 것을 명시
  - AI의 흔한 실수 방지

  4. 특히 효과적인 부분

  1. PRP 시스템: 각 기능을 위한 완전한 "컨텍스트 캡슐" 생성
  2. TASK.md: 개발 히스토리 유지로 중복 작업 방지
  3. 교차 참조: 파일들이 서로를 참조하며 컨텍스트 웹 형성
  4. 플랫폼별 설정: macOS/Windows 빌드 경로 명시

  Context Engineering vs Prompt Engineering 비교

  Prompt Engineering (기존 방식):
  "HP 포션 기능을 추가해줘"

  Context Engineering (이 프로젝트):
  - CLAUDE.md로 전체 프로젝트 규칙 설정
  - PLANNING.md로 아키텍처 이해
  - RAYLIB_CONVENTIONS.md로 코딩 스타일 통일
  - PRPs/hp-potion-item.md로 완전한 구현 청사진 제공
  - 검증 게이트와 테스트 템플릿 포함

  핵심 인사이트

  이 프로젝트는 "Context Engineering is 10x better than prompt engineering"을 실제로 구현한 좋은 예시입니다.
   단순히 요청하는 것이 아니라, AI가 성공적으로 작업할 수 있는 완전한 환경을 구축했습니다.

  가장 혁신적인 점은 각 기능마다 요구사항, 구현 청사진, 검증 전략, 안티패턴을 모두 포함한 PRP를 작성하여,
  AI가 최소한의 인간 개입으로도 복잡한 코드베이스에서 효과적으로 작업할 수 있도록 한 것입니다.
```