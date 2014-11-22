/**
 * Created by ozt on 2014-09-30.
 */


function Poo() {
    this.posX = 0;
    this.posY = 0;
    this.width = 30;
    this.height = 30;
    this.vX = 0;
    this.vY = 0.2;
    this.isVisible = true;
    this.image = new Image();
    this.image.src = "http://www.charbase.com/images/glyph/128169";
}


Poo.prototype.update = function(time){
    if(this.posY > canvas.height){
        this.isVisible = false;
    }
    this.posY += this.vY*time;
};

Poo.prototype.init = function(){
    this.posX = Math.random()*canvas.width;
    this.posY = 0;
    this.isVisible = true;
};

Poo.prototype.render = function(){
    context.drawImage(this.image, this.posX - this.width/2, this.posY - this.height/2, this.width, this.height);
};

Poo.prototype.hitCheck = function(x, y, radius){
 return Math.pow(this.posX - x, 2) + Math.pow(this.posY - y, 2) < Math.pow(this.height + radius, 2)
};