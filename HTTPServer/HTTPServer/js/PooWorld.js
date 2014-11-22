/**
 * Created by ozt on 2014-09-30.
 */
var PooWorld = {};
var canvas = document.getElementById("canvas");
var context = canvas.getContext("2d");
var deltaT = 16;

PooWorld.character = new Character();
PooWorld.background = new PooBackground();
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
    checkTime: 1000,
    PooNum: 0,
    update: function(time){
        this.accTime += time;
        if(this.accTime > this.checkTime){
            this.accTime = 0;
            if(this.checkTime > 200){
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
    this.background.render();
    this.character.render();
    for(var i = 0 ; i < this.pooList.length ; ++i){
        this.pooList[i].render();
    }
    context.font = "30px Arial";
    context.fillStyle = 'black';
    context.fillText(this.score.toString(),10,50);
};




