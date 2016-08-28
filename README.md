# CAAI Abema kun

## 現状
abema くんの鼻が Web カメラに写った顔の鼻の位置にリアルタイムに追従してくる

## ビルド・実行
```
git clone https://github.com/mulgray/caai.git
cd caai/abema/build
cmake .. -DUSE_AVX_INSTRUCTIONS=1
cmake --build . --config Release

./webcam_face_pose_ex
```

## Linux の場合に必要な手順
```
sudo apt-get install cmake libboost-dev libopencv-dev
```

## Mac OS の場合に必要な手順
### 各種ライブラリのインストール
- homebrew
- command line tools in Xcode
- cmake
```
brew install cmake
```
- boost
```
brew install boost
```
- opencv
```
brew tap homebrew/science
brew install opencv
```

### X11のインストール
XQuartz http://www.xquartz.org/ から .dmg をダウンロードしてインストール
インストールしたら Xlib.h が見えるように下記のようにシンボリックリンクを張る
```
$ ln -s /opt/X11/include/X11 /usr/local/include/X11
```

