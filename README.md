# CUDA-Permutation([Game-Simulation](https://github.com/Li-AnLin/Game-Simulation)) 對照組

不用CUDA模擬 [Slot machine](https://www.slotsmillion.com/en/games/) 的排列組合，並能夠模擬指定盤面的出現的機率。

## Input

定義符號和轉盤上的版面及數量，以及中獎盤面的檔案路徑。

[input.csv(暫定格式)](data/input.csv)

![input.csv](data/ScreenShot/input.png)


### Winning Set

用**符號的索引值**定義中獎的盤面

*-1 代表任何相同的圖案*

![W1.txt](data/ScreenShot/W1.png)

## Output

輸出中獎的機率。

[output.csv(暫定格式)](data/output.csv)

![output.csv](data/ScreenShot/output without CUDA.png)

執行時間27s，大約是用CUDA的27倍  

---------------------------------------------
# Debug紀錄 (實驗組)

