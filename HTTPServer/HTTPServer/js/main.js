var stats = new Stats();
stats.setMode(1); // 0: fps, 1: ms

// Align top-left
stats.domElement.style.position = 'absolute';
stats.domElement.style.left = '0px';
stats.domElement.style.top = '0px';

document.body.appendChild( stats.domElement );
PooWorld.init();
setInterval( function () {

    stats.begin();

    context.clearRect(0, 0, canvas.width, canvas.height)
    PooWorld.update(deltaT);
    PooWorld.render();

    stats.end();

}, deltaT );

