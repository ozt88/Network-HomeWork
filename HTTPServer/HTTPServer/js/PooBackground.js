/**
 * Created by ozt on 2014-10-06.
 */
function PooBackground(){
    this.width = canvas.width;
    this.height = canvas.height;
    this.posX = 0;
    this.posY = 0;
    this.image = new Image();
    this.image.src = "https://az31353.vo.msecnd.net/pub/xjkkcfmn";
}

PooBackground.prototype.render = function(){
    context.drawImage(this.image, this.posX, this.posY, this.width, this.height);
}