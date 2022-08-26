<p align="center">
<img align="center" width="356" height="168" src="https://raw.githubusercontent.com/AM71113363/SrtSync/master/info.png">
</p>

# Subtitle(.srt) Sync
-----

## How To Use It?<br>
1. DragDrop a .srt file.<br>
2. If you know the SyncValue(+/- seconds) then you can skip to step 7.<br>
3. Write in the SearchField a phrase(4<length<33) and press ENTER.<br>
4. Click "Prev" or "Next" until you see the right dialog.<br>
5. Look at the movie TIMER an set the values in the RealTime TAB (HH:MM:SS) and click "GET Sync".<br>
6. You can use that value or you can change it if you want.<br>
7. Click "Sync" to save it,the saved filename will be random(Well,I'm using GetTickCount).<br>
8. Rename the new .srt file and try it.<br>

## Build.
_for beginners_ <br>
1.Download [DevCpp_v4.9.9.*](http://www.bloodshed.net/) and install it.<br>
2.Run build.bat (it works only with Dev-C++ 4.9.9.*).<br>
_others_ <br>
You already know how to rebuilt it. ^_^<br>
<br>

# NOTE
If the new file sync is not perfect,then try to use a value +/- 1<br>
The Value to Sync must be in SECONDS.<br>
A negative value means the Subtitle dialog will showUp sooner<br>
And a positive value means the Subtitle dialog will showUp later<br>
*The Phrase to search can be anywhere in the movie dialog,but must be in the same row<br>
*Same Row: if the frame dialog contains two rows and the first half of the phrase to search is 
in the first_row and the second half in second_row,then the SearchFunction will fail.<br>

# SUBNOTE
*I've made this App because My TV supports Subtitle Sync +/-10 , and I need it to be 15<br>
*The dragdrop File ext doesn't have to be .srt,important is to have a .srt file content.<br>
*Example: you can rename your file MovieName.org and the saved files to MovieName.srt

