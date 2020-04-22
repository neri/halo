# HALO: はりぼてOS ローダー for EFI

# これはなに？

UEFI環境ではりぼてOS系のOSをロードします。

[はりぼてOS](http://hrb.osask.jp/) をベースにして [bhsファイル](http://hrb.osask.jp/wiki/?advance/osselect) を公開しているほとんどの OS を無修正で起動することができますが、とりあえず起動できるだけで実用できる組み合わせは限られます。


## ご注意

* **全ての環境の動作を保証するものではありません。**
* 実機で本プログラムを実行するには Secure Boot の設定変更が必要な場合があります。 Secure Boot の設定を変更するするリスクを理解したうえで自己責任で実行してください。
* インストールするには 32GB 以下の FAT 形式でフォーマットされた USB メモリが必要です。
* **32bit カラーモードをサポートしていない OS は描画が乱れるか全く表示されません。** 残念ながら現状ほとんどのOSは 32bit カラーモードに対応していません。 [32bitカラー対応の例1](https://github.com/neri/harib27f_32bpp) / [32bitカラー対応の例2](https://github.com/neri/harib27f_bpp32-b)
* **PS/2 キーボード/マウスをサポートしない機種では操作できません。** 機種によっては起動中に停止します。
* BIOS に依存する OS (CHNOS 等) は正しく動作しません。
* Intel Mac, Hyper-V は正しく動作しません。


# 使い方

* 空いている USB メモリを用意します。 2MB 程度の空き容量があれば最低限の機能をサポートしますが、可能であれば他の用途に使っていない専用のメモリを用意してください。
* \EFI\BOOT\BOOTX64.EFI をコピーしてください。
* 起動したい OS の bhs ファイルと img ファイルをそれぞれ \EFI\halo\haribote.bhs と \EFI\halo\haribote.img にコピーしてください。
* コンピュータを再起動してUSBメモリから起動してください。


# ビルド方法

## 必要なもの

* Linux
* rake (+ruby)
* llvm (clang/lld)
* nasm

## 手順

* Linux 上の作業ディレクトリにレポジトリを展開したあと rake を実行してビルドしてください。
* できあがった /bin/bootx64.efi を USB メモリの \EFI\BOOT\BOOTX64.EFI にコピーします。
