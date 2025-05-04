## Speed comparision (microseconds per 100000 operations on ESP32C3@160MHz)
<table>
<thead>
<th><th>addition</th><th>subtraction</th><th>multiplication</th><th>division</th><th>sin</th><th>sqrt</th><th>asin</th><th>log</th><th>exp</th></tr>
</thead>
<tbody>
<tr><th>fixed32</th><td>6919</td><td>6922</td><td>5034</td><td>38996</td><td>205598</td><td>70137</td><td>144180</td><td>697098</td><td>289895</td></tr>
<tr><th>fixed32_a</th><td>6940</td><td>6920</td><td>15723</td><td>201881</td><td>317254</td><td>258456</td><td>211013</td><td>1277095</td><td>379804</td></tr>
<tr><th>fixed64</th><td>11346</td><td>12579</td><td>15097</td><td>204423</td><td>879286</td><td>369514</td><td>566683</td><td>5165186</td><td>1405123</td></tr>
<tr><th>float</th><td>102087</td><td>106467</td><td>163522</td><td>255331</td><td>2047577</td><td>347389</td><td>537886</td><td>1543864</td><td>2036302</td></tr>
<tr><th>double</th><td>118814</td><td>121305</td><td>288671</td><td>494605</td><td>3052318</td><td>627810</td><td>846876</td><td>2358299</td><td>3022956</td></tr>
<tr><td></td></tr>
<tr><td>
</tbody>
</table>
