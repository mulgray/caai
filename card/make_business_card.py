# --- coding: utf-8 ---
from PIL import Image, ImageDraw, ImageFont
import os
import sys

"""Usage this file.

Requirement:
    This program uses 'pillow', so you need pillow installation.
    http://pillow.readthedocs.io/en/3.3.x/installation.html

Usage:
    python make_business_card.py "野村" "将寛" "ソフトウェアエンジニア"

"""
if len(sys.argv) < 4: # 引数が足りない
    print("The argument is insufficient, so program will exit.")
    sys.exit(1)

def abs_path(filepath):
    return os.path.join(os.path.dirname(os.path.abspath(__file__)), filepath)

# 画像オブジェクト作成
canvas = Image.new('RGB', (1300, 2150), (255, 255, 255))
draw = ImageDraw.Draw(canvas)

# フォントの種類とサイズを指定
font = ImageFont.truetype(abs_path('fonts/hiragino.otf'), int(25 * 4.727))
font2 = ImageFont.truetype(abs_path('fonts/hiragino.otf'), int(8 * 4.727))

familyName = sys.argv[1]
firstName = sys.argv[2]
description = sys.argv[3]

if sys.version_info < (3, 0, 0):
  familyName = sys.argv[1].decode('utf-8')
  firstName = sys.argv[2].decode('utf-8')
  description = sys.argv[3].decode('utf-8')

# テキストを書き込み. 引数は順に, 書き込み座標, テキスト, テキストのフォント, テキストのカラー
y, dy, dyy = 30 * 4.727, 30 * 4.727, 20 * 4.727
for i in range(len(familyName)):
    y += dy
    draw.text((125 * 4.727, y), familyName[i], font=font, fill='black')
y += dyy
for i in range(len(firstName)):
    y += dy
    draw.text((125 * 4.727, y), firstName[i], font=font, fill='black')

y, dy = 15 * 4.727, 10 * 4.727
for i in range(len(description)):
    y += dy
    draw.text((165 * 4.727, y), description[i], font=font2, fill='black')

input_img = Image.open(abs_path('../client/abema.png'), 'r')

canvas.paste(input_img, (int(43 * 4.727), int(250 * 4.727)))

canvas.save(abs_path('img_out.jpg'), 'JPEG', quality=100, optimize=True)

