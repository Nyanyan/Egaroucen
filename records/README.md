English follows Japanese

# Egaroucenによる自己対戦の棋譜

負けを目指すオセロAIは作る人が少ないために、評価関数生成に使えるような形でまとまった量の棋譜が公開されていません。これに問題を感じたので、私の負けオセロAI"Egaroucen"同士による自己対戦の棋譜を公開します。これはEgaroucenの評価関数の学習に使ったものと同じです。



## 内容

各7桁の番号がついたフォルダ内にテキストファイルとして棋譜が入っています。棋譜は一般的なf5d6形式です。

番号が若い方が初期のデータで、精度は良くないです。特に0000000フォルダの棋譜は多分ランダム打ちだと思います。

それぞれのフォルダに数千局の棋譜が入っているので、深層学習などをするには十分な量だと思います。

各フォルダで若干棋譜の細かな仕様が違います。

| フォルダ番号    | 仕様                                                 |
| --------------- | ---------------------------------------------------- |
| 0000000-0000009 | 詳細不明 (忘れました)                                |
| 0000010-0000013 | 中盤7手/終盤14手読み、最大で序盤15手までランダム打ち |
| 0000014-0000016 | 中盤8手/終盤16手読み、最大で序盤15手までランダム打ち |



## ライセンス

このレポジトリにはGPLv3のライセンスがついていますが、この棋譜については全ての権利を放棄します。何に使っていただいても構いませんが、何が起きても保証はしません。もしこの棋譜が役に立ったら連絡をしていただけると私が喜びます。

- Twitter: https://twitter.com/takuto_yamana または https://twitter.com/Nyanyan_Cube
- Instagram: https://www.instagram.com/takutoyamana/
- Facebook: https://www.facebook.com/takutoyamana
- YouTube: https://www.youtube.com/channel/UCOMdTI1hhcNsWI3w4FkT2IQ
- Website: https://nyanyan.github.io/



# Egaroucen's self-play records

There is no records of "weak othello" available on the Internet because very few people create "weak othello" AI. I think this is an issue and I publish my "weak othello" AI, Egaroucen's self-play records. These records were used when creating Egaroucen's evaluation function.



## Details

There are records in txt files with "f5d6" format in each directory with a number.

The smaller the number is, the earlier the records created. For example, records in 0000000 directory are random moves. So please use records in directories with bigger number first.

Each directory has thousands of records. I think this number is enough to do deep learning.

Details of each directory:

| Number          | Details                                                      |
| --------------- | ------------------------------------------------------------ |
| 0000000-0000009 | Unknown (because I forgot)                                   |
| 0000010-0000013 | lookahead is 7 moves in midgame, 14 moves in endgame, has random moves in at most first 15 moves |
| 0000014-0000016 | lookahead is 8 moves in midgame, 16 moves in endgame, has random moves in at most first 15 moves |



## License

This repository is published under GPLv3 license, but these records are published in totally free license. You can use these records in any purposes. There is no warranty. It makes me happy if you tell me you used these records.

- Twitter: either https://twitter.com/takuto_yamana or https://twitter.com/Nyanyan_Cube
- Instagram: https://www.instagram.com/takutoyamana/
- Facebook: https://www.facebook.com/takutoyamana
- YouTube: https://www.youtube.com/channel/UCOMdTI1hhcNsWI3w4FkT2IQ
- Website: https://nyanyan.github.io/