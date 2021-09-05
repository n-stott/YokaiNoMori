function dragStart(e) {
    console.log("drag start : ", e.target, e.target.parentElement.id);
    e.dataTransfer.setData('text/plain', e.target.id + " " + e.target.parentElement.id);
    setTimeout(() => {
        e.target.classList.add('hide');
    }, 0);
}

function dragEnter(e) {
    e.preventDefault();
    e.target.classList.add('drag-over');
}

function dragOver(e) {
    e.preventDefault();
    e.target.classList.add('drag-over');
}

function dragLeave(e) {
    e.target.classList.remove('drag-over');
}

function dragEnd(e) {
    e.preventDefault();

    const data = e.dataTransfer.getData('text/plain');
    id = data.split(" ")[0];
    parentId = data.split(" ")[1];

    const draggable = document.getElementById(id);
    if(draggable) draggable.classList.remove('hide');
}

function drop(e) {
    e.preventDefault();
    var tile;
    if(e.target.classList.contains('tile')) {
        tile = e.target;
    } else {
        tile = e.target.parentElement;
    }
    tile.classList.remove('drag-over');

    // // get the draggable element
    const data = e.dataTransfer.getData('text/plain');
    id = data.split(" ")[0];
    parentId = data.split(" ")[1];
    const draggable = document.getElementById(id);

    srcId = parentId;
    dstId = tile.id;

    tile.appendChild(draggable);        

    // // display the draggable element
    draggable.classList.remove('hide');

    play(srcId, dstId);

}




// const pieces = document.querySelectorAll('.piece');

// pieces.forEach(piece => {
//     piece.addEventListener('dragstart', dragStart);
//     piece.addEventListener('dragend', dragEnd);
// });