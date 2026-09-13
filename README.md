# htop_win
This program is homage of "htop" of linux. 
このプログラムは、linuxのhtopをwindowsで再現できるプログラムです。
ほとんどがバイブコーディングで書かれ、作者はコードのほとんどの意味を理解していません。

linuxには、htopというコマンドがあると思いますが、あれをwindowsでも再現したかったので作りました。
最初はポインタとクラスとヘッダファイルの理解のために書いていたのですが、勢いで作りましたし、前々から欲しかった機能ではあります。

This program is a tribute to the Linux tool "htop."
It is designed to recreate the functionality of Linux's htop on Windows.
Most of it was written using "vibe coding" (AI-assisted coding), and the author does not fully understand the meaning of much of the code.

I created this because I wanted to bring the Linux command "htop" to Windows.
I initially started writing it to better understand pointers, classes, and header files, but I ended up building the whole thing on impulse—plus, it was a tool I had wanted for a long time.

# htop_win

> A tiny `htop`-like process monitor for Windows.

Windowsのターミナルから、動作中のプロセスをシンプルに確認するための小さなシステムモニターです。

Linuxの `htop` に影響を受け、WindowsでもターミナルからCPU・メモリ・プロセスなどを確認できることを目指しています。

```text
┌──────────────────────────────────────────────────────────────┐
│ htop_win                                                     │
├──────────────────────────────────────────────────────────────┤
│ PID      CPU       MEMORY       NAME                         │
│                                                              │
│ 4820     12.4%     182 MB       chrome.exe                   │
│ 9132      4.1%      64 MB       explorer.exe                 │
│ 7420      1.7%      32 MB       WindowsTerminal.exe          │
│ 1337      0.2%      12 MB       htop_win.exe                 │
│                                                              │
└──────────────────────────────────────────────────────────────┘
```

## Features

* Windowsプロセスの一覧表示
* PIDの表示
* CPU使用率の表示
* メモリ使用量の表示
* プロセス名の表示
* ターミナル上でのプロセス監視

シンプルで軽量なターミナルツールを目指しています。

## Installation

Clone the repository:

```powershell
git clone https://github.com/eeemo-hoge/htop_win.git
cd htop_win
```

ソースコードは以下にあります。

```text
src/
└── v1.0/
```

## Usage

ビルド後、実行ファイルを起動します。

```powershell
.\htop_win.exe
```

起動すると、Windows上で動作しているプロセスの情報がターミナルに表示されます。

## Concept

WindowsにはTask Managerがあります。

一方で、ターミナル中心の環境では、GUIを開かずにプロセスを確認したい場合があります。

`htop_win` は、そのような用途のための小さなツールです。

```text
terminal
   │
   ▼
htop_win
   │
   ├── processes
   ├── CPU
   ├── memory
   └── PID
```

機能を増やすことよりも、ターミナルから素早くシステムの状態を確認できることを重視しています。

## Roadmap

* [ ] CPU使用率によるソート
* [ ] メモリ使用量によるソート
* [ ] プロセス検索
* [ ] プロセス終了
* [ ] プロセスツリー表示
* [ ] CPUコアごとの使用率
* [ ] ネットワーク使用量
* [ ] ディスクI/O
* [ ] カラー表示
* [ ] キーボードショートカット

## Project Status

**v1.0 — Early Development**

まだ初期段階のプロジェクトです。

今後、UI・機能・内部構造などが変更される可能性があります。

## Inspiration

This project is inspired by [`htop`](https://htop.dev/).

`htop` のように、ターミナル上でシステムとプロセスの状態を確認できるWindows向けツールを目指しています。

## License

MIT License
