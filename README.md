# First Raylib Project

이 프로젝트는 Raylib 라이브러리를 사용한 간단한 예제입니다.

## 요구 사항

- Raylib 라이브러리 (https://www.raylib.com/)
- GCC 컴파일러
- macOS (현재 Makefile이 macOS에 맞춰져 있습니다)

## Raylib 설치하기

### macOS에서 설치 (Homebrew 사용)

```bash
brew install raylib
```

### 다른 플랫폼에서 설치

다른 플랫폼에서의 설치 방법은 [Raylib 다운로드 페이지](https://www.raylib.com/games.html)를 참조하세요.

## 프로젝트 구조

- `main.c`: 메인 소스 코드 파일
- `Makefile`: 프로젝트 빌드 스크립트

## 컴파일 및 실행

프로젝트를 컴파일하고 실행하려면:

```bash
# 컴파일하기
make

# 실행하기
make run

# 또는 직접 실행
./game
```

## 프로젝트 정리

```bash
make clean
```

## 다음 단계

- 이 프로젝트를 기반으로 게임 기능을 확장해 보세요.
- Raylib의 다른 기능들을 시험해 보세요(텍스처, 사운드, 3D 등).
- Raylib [치트시트](https://www.raylib.com/cheatsheet/cheatsheet.html)를 참조하여 더 많은 기능을 알아보세요. 