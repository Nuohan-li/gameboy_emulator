// V-BLANK: 
// The gameboy draws the display a scanline at a time. There are 144 scanlines on the display 
// and when it has drawn the last one it starts again from the beginning. The time it takes to 
// stop drawing scanline 144 and starting again at scanline 0 is the Vertical Blank period and this 
// is when it requests the v-blank interupt. This is the most important interupt to emulate correctly 
// because during V-Blank the game can read from memory that was previously restricted, mainly video 
// memory. As stated previously the gameboy has a vertical refresh rate of 60 frames per second meaning 
// that if Step 2 and Step 3 of the above steps is always true then there should be 60 V-Blank interupts 
// a second. You will want to monitor this to make sure you are accurately emulating V-Blank.

// LCD: 
// There are many reasons why the LCD would request an interupt and these will be looked at
// in more detail in the next chapter called LCD. The main info you need to know about the LCD 
// interupt is it lets the game know what state the LCD is in because depending what state its 
// in certain memory regions become restricted. The game can also set up a conincidence variable 
// which means "let me know when you're active scanline is X". One of the things that stumped me 
// with the LCD interupt is that it can request an LCD interupt whenever V-Blank happens. I didnt 
// understand why there were two v-blank interupts, the main one which was discussed above and the 
// one nested into the LCD interupt. What you need to remember is that during V-Blank if the game is 
// allowing it there will be two V-Blank interupts requested. The first is the main V-Blank interupt 
// and the second is the LCD interupt. However the first vblank interupt will have the higher priority.

// TIMER: 
// This interupt has been discussed previously in the timers section. Basically the gameboy timer counts 
// up at a dynamic frequency and when it gets to value 255 and is about to overflow it lets the game know 
// by requesting the timer interupt.

// JOYPAD: 
// The joypad will be discussed later on in the chapter Joypad. This interupt is requested whenever one 
// of the buttons goes from the unpressed state to the pressed state. 

