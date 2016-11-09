HALO: はりぼてOS ローダー for EFI
====

# これはなに？

UEFI環境ではりぼてOS系のOSをロードします。

はりぼてOS (http://hrb.osask.jp/) をベースにしてbhsファイルを公開しているほとんどのOSを無修正で起動することができますが、以下の制限があるので実用できる組み合わせは限られます。

* PS/2ポートをサポートしない機種では操作できなかったり起動しない場合があります。
* 32bit カラーモードをサポートしていないOSは描画が乱れるか全く表示されません。残念ながら現状ほとんどのOSは 32bit カラーモードに対応していません。
* [32bitカラー対応OSのサンプル](https://github.com/neri/harib27f_bpp32)

# ご注意

* **全ての環境の動作を保証するものではありません。**
* **64bit UEFI専用です。** 64bit CPUを搭載していてもUEFIのモードが32bitの機種はサポートしていません。 Windows 管理ツール→システム情報→システムの要約で「BIOSモード」が「UEFI」、「システムの種類」が「x64-ベースPC」になってることを確認してください。
* 本プログラムを起動するときは UEFI 設定で Secure Boot を無効にしてください。
* インストールするメディアは 32GB 以下の FAT 形式でフォーマットされたメディアが必要です。


# 使い方

* 空いているUSBメモリを用意します。 2MB 程度の空き容量があれば最低限の機能をサポートしますが、可能であれば他の用途に使っていない専用のメモリを用意してください。
* \EFI\boot\bootx64.efi をコピーしてください。
* 起動したいOSのbhsファイルとimgファイルをそれぞれ \EFI\halo\haribote.bhs と \EFI\halo\haribote.img にコピーしてください。
* コンピュータを再起動してUSBメモリから起動してください。


# ビルド方法

## 必要なもの

* Linux (or Bash on Windows)
* rake (+ruby)
* clang (or gcc)
* binutils
* gnu-efi
* nasm

## 手順

* Linux (Bash on WindowsでもOK) 上の作業ディレクトリにレポジトリを展開したあと rake を実行してビルドしてください。
* できあがった /bin/bootx64.efi を USB メモリの \EFI\boot\bootx64.efi にコピーします。
* 起動したいOSのbhsファイルとイメージファイルをそれぞれ \EFI\halo\haribote.bhs と \EFI\halo\haribote.img にコピーしてください。
* コンピュータを再起動してUSBメモリから起動してください。


# そのうちできたらいいな

* 起動するOSを固定ファイル名から変更できるようにしたい
* 起動したOSをUEFI-Awareに対応したい
