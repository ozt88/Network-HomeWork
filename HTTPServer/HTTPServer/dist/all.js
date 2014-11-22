/**
 * Created by ozt on 2014-09-30.
 */
function Character() {
    this.posX = canvas.width/2;
    this.posY = canvas.height/2;
    this.width = 10;
    this.height = 10;
    this.vX = 0;
    this.vY = 0;
    this.accX = 0;
    this.accY = 0;
    this.isAlive = true;
    this.image = new Image();
    this.image.src = "http://img2.wikia.nocookie.net/__cb20110324230458/kirby/en/images/3/32/Kirby_character.png";
}

Character.prototype.move = function(evt) {
   var keyCode = evt.keyCode;
        if (keyCode == 37){
               this.accX = -0.03;
        }
        if (keyCode == 39){
            this.accX = 0.03;
        }
        if (keyCode == 38) {
            this.accY = -0.03;
        }
        if (keyCode == 40) {
            this.accY = 0.03;
        }
};

Character.prototype.stop = function(){
    this.accX = 0;
    this.accY = 0;
};

Character.prototype.friction = function(velocity){
    var frictionAcc = 0;
    if(velocity > 0 ){
        frictionAcc = -0.01;
    }
    else if(velocity < 0){
        frictionAcc = 0.01;
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

var xd = []
Character.prototype.render = function(){
    if(this.isAlive) {
        context.drawImage(this.image, this.posX - this.width/2, this.posY - this.height/2, this.width*4, this.height*4);
    }
};

/**
 * Created by ozt on 2014-09-30.
 */


function Poo() {
    this.posX = 0;
    this.posY = 0;
    this.width = 10;
    this.height = 10;
    this.vX = 0;
    this.vY = 2.5;
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
/**
 * Created by ozt on 2014-09-30.
 */
var PooWorld = {};
var canvas = document.querySelector("canvas");
var context = canvas.getContext("2d");
var deltaT = 60;

PooWorld.character = new Character();
PooWorld.pooList = [];
PooWorld.score = 0;
PooWorld.width = canvas.width;
PooWorld.height = canvas.height;
PooWorld.isGameEnd = false;


function onKeyDown(e){
    console.log(e);
    PooWorld.character.move(e);
}
function onKeyUp(){
    PooWorld.character.stop();
}
document.addEventListener('keydown', onKeyDown);
document.addEventListener('keyup', onKeyUp);

PooWorld.PooFactory = {
    accTime: 0,
    checkTime: 300,
    PooNum: 0,
    update: function(time){
        this.accTime += time;
        if(this.accTime > this.checkTime){
            this.accTime = 0;
            if(this.checkTime > 20){
                this.checkTime -= this.checkTime/10;
            }
            PooWorld.addPoo();
        }
    }
};

PooWorld.collisionCheck = function(){
    for(var i = 0 ; i < this.pooList.length; ++i){
        if(this.pooList[i].hitCheck(this.character.posX, this.character.posY, this.character.height))
        {
            this.pooList[i].isVisible = false;
            this.character.isAlive = false;
            this.isGameEnd = true;
        }
    }
};

PooWorld.addPoo = function(){
      var newPoo = new Poo();
      newPoo.init();
      this.pooList.push(newPoo);
};

PooWorld.init = function(){
    this.character.init();
};

PooWorld.update = function(time){
    if(this.isGameEnd == false)
    {
        this.collisionCheck();
        this.PooFactory.update(time);
        this.character.update(time);
        for(var i = 0 ; i < this.pooList.length ; ++i){
            this.pooList[i].update(time);
            if( this.pooList[i].isVisible == false) {
                this.pooList.splice(i, 1);
                this.score++;
            }
        }
    }
};

PooWorld.render = function(){
    this.character.render();
    for(var i = 0 ; i < this.pooList.length ; ++i){
        this.pooList[i].render();
    }
    context.font = "30px Arial";
    context.fillStyle = 'black';
    context.fillText(this.score.toString(),10,50);
};





/**
 * Created by ozt on 2014-10-02.
 */
(function(){
    document.body.style.width = '100%';
    document.body.style.height = '100%';
    document.body.style.margin = '0';
    document.body.style.overflow = 'hidden';
    var canvas = document.getElementById('canvas');
    function resizeCanvas() {
        canvas.width = window.innerWidth;
        canvas.height = window.innerHeight;
    }
    resizeCanvas();
    window.addEventListener('resize', resizeCanvas);
})();

PooWorld.init();
var MainInterval = setInterval(function() {
        context.fillStyle = 'white';
        context.fillRect(0, 0, canvas.width, canvas.height);
        PooWorld.update(deltaT);
        PooWorld.render();
    }
    ,deltaT);


