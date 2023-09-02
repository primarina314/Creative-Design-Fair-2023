# Creative-Design-Fair-2023
팀명: 탑골공원

Team: Tap-gol Park

# Summary

<img width="957" alt="Image-Description" src="https://github.com/primarina314/Creative-Design-Fair-2023/assets/102790490/71784abf-33b2-403c-a57b-1f3115edc6e3">

주변의 정자나 공원에 가면 노인분들이 다같이 모여 장기를 두고 계시는 모습을 심심치 않게 볼 수 있다. 하지만 장기를 두는 곳이 야외이기 때문에 날씨가 좋지 않은 경우 노인분들이 취미를 즐기시기 힘들 수 있다. 또 거동이 불편하여 외출이 힘드신 경우도 존재한다. 물론 인터넷 장기 게임이라는 대체재가 존재하기는 하지만 노인분들의 경우 디지털 매체에 익숙하지 않으시다는 문제가 있고 보드게임 특성상 실제 기물과 판을 사용하는 것을 선호하는 사람이 더 많다. 이를 위해 사용자들이 원격으로 장기를 둘 수 있도록 게임 진행에 따라 장기말이 자동으로 이동하는 판을 제작하고자 하였다.

사용자가 말을 움직이면 상단에 위치한 카메라를 통해 기물의 이동을 인식하고 이를 인식하고 openCV 프로그램으로 초기 좌표와 움직이고 난 후의 좌표를 도출한다. 이 좌표는 firebase 서버에 전송되고, 이후 이 데이터가 상대방의 서버로 전송된다. 이 데이터에 따라 장기판 하단에 위치하고 있는 XY플로터가 작동하게 되고 장기말이 서로 부딪히지 않는 최적의 경로를 파악하여 말이 움직이게 된다. 이 과정이 반복되어 게임이 진행된다. 이때 자동 장기판과 직접 제작한 온라인 장기 게임 프로그램과 연동하여 사용도 가능하다. 이러한 특징 덕분에 실제 기물을 선호하는 사용자와 온라인으로 장기를 두는 사용자 모두의 니즈를 충족시킬 수 있다. 

이 장기판의 가장 큰 기대효과는 실내에서 원격으로 타인과 장기를 둘 수 있다는 것이다. 초반에 언급했듯이 장기판의 주 사용 계층으로 예상되는 노인분들은 거동이 불편하신 분들이 다수 존재하는데 이러한 분들도 이동의 부담 없이 취미생활을 즐기실 수 있다. 또한, 한여름이나 한겨울, 악천후와 같이 외출이 힘든 상황에서도 장기를 두는 것이 가능하다는 장점이 있다.

# Environment
> arduino mega 2560
>
> PlatformIO
>platform = atmelavr
> 
> board = megaatmega2560
> 
> framework = arduino
> 
> lib_deps = arduino-libraries/Servo@^1.2.1
> 
> 
> opencv-python 4.8.0.76
> 
> pyserial 3.5
> 
> pygame 2.5.1
> 
> termcolor 2.3.0
> 
> firebase-admin 6.2.0
>
> ipykernel 6.25.1
> 
> Works on jupyter notebook

# 기타
> 'PATH TO YOUR KEY' 와 'URL OF YOUR DB' 에 각각 firebase 접속키(json) 경로와 URL 주소를 입력해야한다.
