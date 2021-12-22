# Personal-Engine

## [윈도우 환경 엔진 테스트용 프로그램](https://github.com/Popcron-s/Personal-Engine/raw/master/Resources/Info-Data/ConsoleTest%20Output.zip)
* <h3>업로드 파일 테스트 일자 : 21 / 09 / 05</h3>
* <h3>테스트 내용</h3>

  1. 폰트 셰이더 테스트
  2. Z sort 셰이더 테스트
  3. Light 셰이더 테스트
  4. 다중 카메라 및 카메라 오더 테스트
  5. gjk-epa 알고리즘을 이용한 2D 충돌 처리 테스트


## 개발 현황 및 개발 예정
<img src="https://raw.githubusercontent.com/Popcron-s/Personal-Engine/3228a0f1ce3592e6f83b4bac8e6ea7428ad03243/Develop%20Simply%20UML.drawio.svg">

## Update 2021-12-23
* 폰트/ttf 알고리즘 수정
* .sln VS2022버전으로 수정
* LocalGit 사용 및 Github 연동

## 정리
* <h3>우선 간략적으로 정리하고 이후 다이어그램 추가와 함께 수정할 예정</h3>

* 플랫폼
  - Windowns 32bit
    - 32bit Windows 플랫폼 생성 모듈
    - 관련 문서
      - Win32/Windows.h
      - Win32/Windows.cpp
  - Android
    - Android 플랫폼 생성 모듈
    - 관련 문서
      - Android/Android.h
      - Android/Android.cpp
* 그래픽스
   - OpenGL
      - Windows용 그래픽 랜더 모듈
      - 관련 문서
        - Win32/OpenGL.h
        - Win32/OpenGL.cpp
   - OpenGLES
      - Android용 그래픽 랜더 모듈
      - 현재 버전의 랜더러와 호환되지 않아 동작을 안함
      - 관련 문서
        - Android/OpenGLES.h
        - Android/OpenGLES.cpp
    - Z-order
      - SHADER를 통한 Z-order
      - 오브젝트 셰이더에서 깊이와 색상을 기록한 플래그먼트 변수를 버퍼에 저장
      - 카메라 셰이더에서 저장된 플래그먼츠를 가져와 깊이별로 재정렬한 후 색상 합성하는 방식을 사용
      - 관련 문서
        - Resources/shader/default_vs.txt
        - Resources/shader/default_fs.txt
        - Resources/shader/cam_vs.txt
        - Resources/shader/cam_fs.txt
    - Font Shader
      - ttf에서 읽어들인 Vector데이터를 이용하여 쉐이더에서 렌더링
      - Box Vertex 셋팅후 fragments shader에서 연산 처리
      - 최초 기획은 fragments shader에서 bezier연산까지 하는거였으나 연산량이 많아질 우려가 있어서 전환
      - ttf decoder에서 bezier를 8분할하여 직선으로 재배치
      - 외곽선은 Point to Line Distance를 사용하여 연산
      - 내부는 Winding Number Algorithm를 사용하여 연산
      - 관련 문서
        - Resources/shader/font_vs.txt
        - Resources/shader/font_fs.txt
        - Coder/TrueTypeDecoder.cpp
          - glyf 모듈 관련 함수
* 사운드
  - XAudio2
    - Windows용 사운드 랜더 모듈
    - 관련 문서
      - Win32/XAudio2.h
  - OpenSLES
    - Android용 사운드 랜더 모듈
    - 관련 문서
      - Android/OpenSLES.h
      - Android/OpenSLES.cpp
* 파일 입출력
  - Windows용 파일 입출력 모듈
    - 관련 문서
      - Win32/FileIO.h
  - Android용 파일 입출력 모듈
    - 관련 문서
      - Android/FileIO.h
      - Android/FileIO.cpp
* Input
  - Windows
    - 키보드 / 마우스의 인풋 모듈
    - 관련 문서
      - Win32/WinInput.h
  - Android
    - 터치 / 자이로 센서의 인풋 모듈
    - 관련 문서
      - Android/AndroidTouch.h
      - Android/GyroSensor.cpp
* 타이머
  - 틱과 시스템 시간을 가져올 수 있는 모듈
    - Windows용 관련 문서
      - Win32/Timer.h
    - Android용 관련 문서
      - Android/Timer.h
      - Android/Timer.cpp
* 메인 루프 시스템
  - Export 함수
    - 라이브러리 제작시 라이브러리와 함께 제공되는 외부 공개용 함수 페이지
    - 오브젝트 클래스, 모듈 클래스, 기본 정의 페이지를 함께 제공한다.
    - 관련 문서
      - BaseLibrary/MainSystem.h
      - BaseLibrary/Object.h
      - BaseLibrary/Module.h
      - Common/variable.h
      - Common/Enum.h
  - Start(*)
    - 프로그램 시작 구성 클래스
    - Android와 Windows의 미세한 생성 차이때문에 같은 함수를 선언하지만 사용하는 라이브러리에 따라 다르게 정의되도록 설정되어있다
    - 관련 문서
      - BaseLibrary/MainSystem.h
      - Win32/MainSystem_win32.cpp
      - Android/MainSystem_android.cpp
  - Run(*)
    - 메인 루프에 들어가는 함수
    - 메인 루프는 이하의 순서를 따라간다
      - 타이머 업데이트
      - 인풋 받아오기
      - 오브젝트 매니저에서 오브젝트 업데이트 및 모듈 분류 수행
        - 해당 과정 중 오브젝트ID 순서로 정렬하여 순서대로 실행하도록 설정
        - 현재 충돌은 업데이트에서 동작하도록 되어있음. 추후 분리 예정
      - 랜더
    - 관련 문서
      - BaseLibrary/MainSystem.h
      - BaseLibrary/MainSystem_common.cpp
  - End(*)
    - 프로그램 종료 선언
    - 모든 모듈의 해제를 담당
    - 관련 문서
      - BaseLibrary/MainSystem.h
      - BaseLibrary/MainSystem_common.cpp
  - 오브젝트 매니저
    - 오브젝트의 통합 관리를 담당
    - 모든 오브젝트는 사용 여부와 별개로 반드시 여기에 등록됨
    - 클래스 릴리즈시 등록된 모든 오브젝트는 자동으로 소멸시켜줌
    - 루프 업데이트에서 오브젝트의 업데이트와 모듈의 분류를 담당
    - 분류된 모듈은 이후 임의의 루프 클래스에 호출되어 사용됨
    - 관련 문서
      - BaseLibrary/ObjectManager.h
      - BaseLibrary/ObjectManager.cpp
  - 리소스 매니저
    - 리소스 일괄 관리. 등록 및 호출을 관리하여 중복된 리소스의 추가 메모리를 방지
    - 현재 STL의 MAP을 쓰고 있으나 추후 BLACK-RED Tree로 변경할 예정
    - 기존에 쓰던 코드는 버그가 있어서 임시 동결
    - 관련 문서
      - BaseLibrary/ResourceManager.h
      - BaseLibrary/ResourceManager.cpp
  - 충돌 클래스
    - GJK-EPA 알고리즘을 사용
    - 최초 최대반경을 통해 1차 판별
    - GJK를 사용하여 2차 판별을 함
    - 이후 EPA사용여부에 따라 추가로 충돌깊이를 반환해줌
    - 현재 루프 업데이트는 사용되지 않는 중
    - 관련 문서
      - BaseLibrary/Collision.h
      - BaseLibrary/Collision.cpp
  - 랜더러
    - 그래픽 랜더러
      - 등록된 매시모듈을 그래픽 랜더 모듈에 넘겨주는 역할
      - 그 과정중 배치를 통해 최대한 드로우 콜을 줄인다.
      - 최대 16개의 텍스처를 배열로 등록하기 때문에 일부 텍스처가 다른 오브젝트도 동일한 셰이더만 사용하면 하나로 배칭 가능하다.
    - 사운드 랜더러
      - 사운드 모듈에서 음성이 재생되고 있는지 판단하는 클래스
      - 현재 제기능을 하고 있지 않음.
    - 관련 문서
      - BaseLibrary/Renderer.h
      - BaseLibrary/Renderer.cpp
* 오브젝트 시스템
  - 오브젝트
    - 오브젝트의 기본이 되는 단위
    - 기본이 되는 조인트를 반드시 소지
    - 모듈 리스트 초기화로 모듈 등록 가능
    - 모듈 리스트를 릴리즈 하면 자동으로 등록된 모듈들이 삭제됨
    - 관련 문서
      - BaseLibrary/Object.h
      - BaseLibrary/Object.cpp
  - 조인트
    - 위치, 회전, 크기 정보를 담고 있는 클래스
    - 월드 매트릭스를 소유하고 있으며 오브젝트와 모듈에 등록되는 형태로 사용
    - 조인트 클래스를 부모로 지정해줄 수 있다.
    - 부모를 지정해줄 경우 루프 시스템을 돌면서 계산할때 같이 계산해준다.
    - 이후 루프를 돌때 월드 매트릭스를 잠궈놓지 않을경우 자동으로 업데이트 해준다.
    - 모듈과 오브젝트의 부모 지정은 전부 조인트를 통해서 이루어진다.
    - 관련 문서
      - BaseLibrary/Module.h
      - BaseLibrary/Module.cpp
  - 모듈
    - 여러가지 기능을 하는 클래스
    - 오브젝트에 등록되서 사용
    - 조인트의 부모지정과는 별개로 자신을 소유한 오브젝트의 부모지정이 가능하다.
    - 별개의 조인트 생성을 하지 않을경우 부모로 지정된 오브젝트의 조인트를 따라간다.
    - 모듈 리스트
      - 내부가 정의된 모듈만 작성
      - Camera
      - PolygonMesh : public Mesh
      - Audio
      - PolygonCollider : public Collider
      - TextMesh : public Mesh
    - 관련 문서
      - BaseLibrary/Module.h
      - BaseLibrary/Module.cpp
* 기타
  - 디코더
    - .BMP
      - 인코더 구현
      - 관련 문서
        - Coder/BMPDecoder.h
        - Coder/BMPDEcoder.cpp
        - Coder/BMPEncoder.h
        - Coder/BMPEncoder.cpp
    - .PNG
      - 관련 문서
        - Coder/PNGDecoder.h
        - Coder/PNGDecoder.cpp
        - Coder/ChunkReader.h
        - Coder/ChunkReader.cpp
        - Coder/Inflate.h
        - Coder/Inflate.cpp
        - Coder/Adler32.h
        - Coder/Adler32.cpp
        - Coder/CRC32.h
        - Coder/CRC32.cpp
    - .WAV
      - 관련 문서
        - Coder/WAVDecoder.h
        - Coder/WAVDecoder.cpp
    - .OGG
      - 일부 형식의 파일만 디코드 가능
      - 관련 문서
        - Coder/OGGDecoder.h
        - Coder/OGGDecoder.cpp
        - Coder/Vorbis.h
        - Coder/Vorbis.cpp
        - Coder/VorbisHeader.h
        - Coder/VorbisHeaderDecoder.cpp
        - Coder/VorbisPacket.h
        - Coder/VorbisPacketDecoder.cpp
        - Coder/VorbisSupporter.h
        - Coder/VorbisSupporter.cpp
    - .TTF
      - 일부 형식의 파일만 디코드 가능
      - 관련 문서
        - Coder/TrueTypeDecoder.h
        - Coder/TrueTypeDecoder.cpp
        - BaseLibrary/Font.h
        - BaseLibrary/Font.cpp
    - .ZIP
      - 관련 문서
        - Coder/ZIPDecoder.h
        - Coder/ZIPDecoder.cpp
        - Coder/Inflate.h
        - Coder/Inflate.cpp
    - .TXT
      - UTF-8(BOM)만 디코드 가능
      - 관련 문서
        - Coder/TextDecoder.h
        - Coder/TextDecoder.cpp
  - 간략한 계산기
    - 쿼터니온 계산기
      - FLOAT3 to Quternion
      - FLOAT3 to Matrix
    - VECTOR 계산기
      - dot product
      - cross product
      - length^2
      - length
      - normalize
      - Bezier curve
      - VECTOR * MATRIX
    - MATRIX 계산기
      - Caculate World Matrix
      - Caculate View Matrix
      - Caculate Ortho Projection Matrix
      - Caculate Perspective Projection Matrix
      - MATRIX * MATRIX
      - REVERSE MATRIX
    - 관련 문서
      - Common/variable.h
