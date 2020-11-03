#!/usr/bin/env python3
# SNDファイルからplayMMLのchar配列文字列を生成する
# print("usage: ./__snd2char.py 入力ファイル 出力ファイル")
import os
import sys


def filechk(infile, outfile):
    # 入力ファイルが無ければエラー
    if not os.path.exists(infile):
        sys.exit("入力ファイルが見つかりません")
    # 出力ファイルが被れば消してしまう
    if os.path.exists(outfile):
        os.remove(outfile)
        print("出力ファイルが被ったので消しました")
    return


def convfile(infile, outfile):
    with open(infile, 'r', encoding='shift_jis') as f:  # 200LXならSJIS
        snd = f.readlines()
    
    with open(outfile, "w") as f:   # UTF-8
        f.write("char *s[]= {\n")
        for row in snd:
            if not row.lower().startswith((':f',':c',':w',':e',':*','\n')):
                # truncate space
                row = '  "' + row[:len(row)-1].replace(' ', '') + '",\n'
                f.write(row)
        f.write('}; // メモリ削減のためコメント等は割愛しました\n')
        f.write('\n')


def catfile(infile):
    with open(infile, 'r') as f:
        txt = f.readlines()
    for row in txt:
        print(row, end='')
    print("text size is ", sum(len(row) for row in txt))


def main():
    # 引数取得 sys.argvで取得した要素の型はstr アンパックを使ってみた
    args = sys.argv
    try:
        command, infile, outfile = args
    except Exception:
        mes = args[0] \
            + " 入力ファイル"
        sys.exit("usage: {}".format(mes))
    # 入出力ファイルのチェック
    filechk(infile, outfile)
    # conversion
    convfile(infile, outfile)
    # cat
    catfile(outfile)
    # 終了処理
    mes = "処理終了！"
    print(mes.format(outfile, outfile))


if __name__ == "__main__":
    main()
