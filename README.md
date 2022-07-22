# ViewAngleDumper

I just made this Dll for the new Recoil update.
I spend 10 minutes or so to Code this, so there might be flaws or inefficiency. (Had no time to do it proper)
Feel free to modify or c+p the code


Credits for the il2cpp stuff to:
- P402

Some things you may change/add:
1. automatically detect new magazine
2. dynamically add patterns to the patterns vector
3. dynamically dump for different weapons
3. I guess you can find ich more to optimize


How to:
1. I would suggest dumping the recoil viewangles on your own server using "eac 0", "encrypted 0" and "secure 0"
2. Start RustClient.exe (under no circumstances Start Rust.exe)
3. Join your Dedicated Server
4. Shoot Magazine
5. Press Mouse 3 (middle mouse) to switch to the next magazine
6. Repeat 4. & 5. up to twenty times
7. Press End to dump the average into C:/**/Rust/dumped.txt
8. Clear dumped.txt when you want to dump another weapon
