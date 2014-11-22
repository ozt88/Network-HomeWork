/**
 * Created by ozt on 2014-09-30.
 */
function Character() {
    this.posX = canvas.width/2;
    this.posY = canvas.height*3/4;
    this.width = 15;
    this.height = 15;
    this.vX = 0;
    this.vY = 0;
    this.accX = 0;
    this.accY = 0;
    this.keyForce = 0.002;
    this.frictionForce = 0.001;
    this.isAlive = true;
    this.image = new Image();
    this.image.src = "http://img2.wikia.nocookie.net/__cb20110324230458/kirby/en/images/3/32/Kirby_character.png";
}

Character.prototype.move = function(evt) {
   var keyCode = evt.keyCode;
        if (keyCode == 37){
            this.accX = -this.keyForce;
        }
        if (keyCode == 39){
            this.accX = this.keyForce;
        }
};

Character.prototype.stop = function(){
    this.accX = 0;
    this.accY = 0;
};

Character.prototype.friction = function(velocity){
    var frictionAcc = 0;
    if(velocity > 0 ){
        frictionAcc = - this.frictionForce;
    }
    else if(velocity < 0){
        frictionAcc =  this.frictionForce;
    }
    return frictionAcc;
};

Character.prototype.init = function(){
    this.isAlive = true;
};

Character.prototype.update = function(time){
    if(Math.abs(this.vX + this.accX) < 100){
        this.vX += (this.accX + this.friction(this.vX))*time;
        this.vY += (this.accY + this.friction(this.vY))*time;
    }
    if(this.posX + this.vX*time > 10 && this.posX + this.vX*time < canvas.width - this.width*4){
        this.posX += this.vX*time;
    }
    if(this.posY + this.vY*time > 10 && this.posY + this.vY*time < canvas.height - this.height*4){
        this.posY += this.vY*time;
    }
};

Character.prototype.render = function(){
    if(this.isAlive) {
        context.drawImage(this.image, this.posX - this.width/2, this.posY - this.height/2, this.width*4, this.height*4);
    }
};
