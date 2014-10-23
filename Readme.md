#PowerPC64Simulator

情報科学基礎実験の課題のPowerPC64のシミュレーター。
x86_64-linux-gnuのgccに依存するコードを含みます(ちなみに自分はgcc (Ubuntu 4.8.2-19ubuntu1) 4.8.2で動作のテストをしています)。
* signed型の算術右シフト
* ビットフィールドの順序
* short, int, long longを2, 4, 8バイトの整数型として扱っている

他にもあるかも

### 実装の方針
* PowerPCの命令を一つずつシミュレートして、レジスタの内容の変化を追えるようなプログラムにする
* 算術演算、論理演算、分岐命令、比較などを中心に実装
* 浮動小数点数は(面倒なので)実装しない
* 例外処理、メモリアクセスの際のアドレス変換、特権モードなども(今のところの予定では)実装しない

### 使い方
#### アセンブリコードの作り方
そのうち書く
#### シミュレーターの使い方
```
make
./powerpc64sim <PPC64のアセンブリコード>
```
#### コマンド一覧
コマンド | 説明
---------|-------------------
h        | ヘルプの表示
r <レジスタ番号> | GPRの中身を表示
n        | 次の命令を実行
d        | 全てのレジスタの中身を表示する
q        | 終了する

### 実装済み命令一覧
#### 算術演算
- [x] addi
- [x] addis
- [x] add
- [x] subf
- [x] addic
- [ ] addic.
- [ ] subfic
- [x] addc
- [ ] subfc
- [x] adde
- [ ] subfe
- [ ] addme
- [ ] subfme
- [ ] addze
- [ ] subfze
- [ ] neg
- [ ] mulli
- [ ] mulhw
- [ ] mullw
- [ ] mulhwu
- [ ] divw
- [ ] divwu
- [ ] divwe
- [ ] divweu
- [ ] mulld
- [ ] mulhd
- [ ] mulhdu
- [ ] divd
- [ ] divdu
- [ ] divde
- [ ] divdeu
#### 論理演算
- [x] andi
- [x] andis
- [x] ori
- [x] oris
- [x] xori
- [x] xoris
- [x] and
- [x] or
- [x] xor
- [x] nand
- [x] nor
- [x] eqv
- [x] andc
- [x] orc
- [x] extsb
- [x] extsh
- [x] cntlzw
- [x] cmpb
- [x] popcntb
- [x] popcntw
- [ ] prtyd
- [ ] prtyw
- [ ] extsw
- [ ] popcntd
- [ ] cntlzd
- [ ] bpermd
#### シフト/ローテート
- [ ] rlwinm
- [ ] rlwnm
- [ ] rlwimi
- [ ] rldicl
- [ ] rldicr
- [ ] rldic
- [ ] rldcl
- [ ] rldcr
- [ ] rldimi
- [ ] slw
- [x] srw
- [x] srawi
- [ ] sraw
- [ ] sld
- [ ] srd
- [ ] sradi
- [ ] srad
#### 分岐命令
- [x] b
- [x] bc
- [x] bclr
- [x] bcctr
#### ロード/ストア
- [x] stb
- [x] stbx
- [x] stbu
- [ ] stbux
- [x] sth
- [ ] sthx
- [x] sthu
- [ ] sthux
- [x] stw
- [ ] stwx
- [x] stwu
- [ ] stwux
- [x] std
- [ ] stdx
- [x] stdu
- [ ] stdux
- [x] lbz
- [x] lbzx
- [x] lbzu
- [x] lbzux
- [x] lhz
- [x] lhzx
- [x] lhzu
- [x] lhzux
- [x] lha
- [x] lhax
- [x] lhau
- [x] lhaux
- [x] lwz
- [x] lwzx
- [x] lwzu
- [x] lwzux
- [x] lwa
- [x] lwax
- [ ] lwaux
- [x] ld
- [ ] ldx
- [ ] ldu
- [ ] ldux
#### Move To/From System Register Instructions
- [ ] mtspr
- [ ] mtcrf
- [ ] mfcr
- [ ] mtocrf
- [ ] mfocrf
- [ ] mcrxr
- [ ] mtdcrux
- [ ] mfdcrux
#### 比較
- [x] cmpi
- [ ] cmp
- [ ] cmpli
- [ ] cmpl
#### Condition Register Instructions
- [x] crand
- [x] crnand
- [x] cror
- [x] crxor
- [x] crnor
- [x] creqv
- [x] crandc
- [x] crorc
- [ ] mcrf


### 実装済みレジスタ一覧
* GPR(General Purpose Register)
* CR(Condition Register)
* LR(Link Register)
* CTR(Count Register)
* XER(Fixed-Point Exception Register)

### 補足(今後変更する可能性が高いことなど)

