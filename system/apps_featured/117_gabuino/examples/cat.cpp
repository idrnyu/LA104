#include <library.h>

uint16_t buffer[320*2];
uint8_t check = 0;

int main(void)
{
    BIOS::DBG::Print(R"(
<canvas id="canvas" width=320 height=240></script>
<script>
var ready = false;
var can = document.getElementById('canvas');
var ctx = can.getContext('2d');
var img = new Image;
var imgData;
img.crossOrigin = "Anonymous";
img.onload = function() {  
  var s = Math.max(can.width/img.width, can.height/img.height);
  var w = img.width*s;
  var h = img.height*s;
  ctx.drawImage(img, (320-w)/2, (240-h)/2, w, h);

  imgData = ctx.getImageData(0, 0, can.width, can.height);
  ready = true;
};
img.src = "https://cataas.com/cat?"+Math.random();

window.transfer = function(left, top, right, bottom, ptr, check) 
{
    if (!ready)
        throw "not ready!";
        
    var buf = [];

    for (var y=top; y<bottom; y++)
        for (var x=left; x<right; x++)
        {
            var r = imgData.data[(y*320+x)*4]; 
            var g = imgData.data[(y*320+x)*4+1]; 
            var b = imgData.data[(y*320+x)*4+2]; 
            var rgb565 = (r>>3) | ((g>>2)<<5) | ((b>>3)<<11);
            buf.push(rgb565 & 255);
            buf.push(rgb565 >> 8);
        }

    return BIOS.memWrite(ptr, buf)
        .then( () => BIOS.memWrite(check, [1]));
}
</script>)");
    
    BIOS::SYS::DelayMs(1000); // TODO: remove
    check = 0;
    int line = 14;    
    CRect rect(0, 0, BIOS::LCD::Width, BIOS::LCD::Height);
    while (BIOS::KEY::GetKey() == BIOS::KEY::EKey::None)
    {
        if (check == 0)
        {
            check = 3;
            rect.top = line;
            rect.bottom = line+1;
            BIOS::DBG::Print(R"(<script>transfer(%d, %d, %d, %d, 0x%08x, 0x%08x)</script>)", 
                rect.left, rect.top, rect.right, rect.bottom, 
                buffer, &check);
                
            if (line++ >= BIOS::LCD::Height-14)
                break;
        }
        if (check == 1)
        {
            //BIOS::SYS::DelayMs(100);
            BIOS::LCD::BufferBegin(rect);
            BIOS::LCD::BufferWrite(buffer, rect.Width()*rect.Height());
            BIOS::LCD::BufferEnd();
            check = 0;
        }
    }
    BIOS::SYS::DelayMs(1000); // TODO: remove
    
    return 0;
}
