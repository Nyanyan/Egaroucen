var hw = 8;
let dy = [0, 1, 0, -1, 1, 1, -1, -1];
let dx = [1, 0, -1, 0, 1, -1, 1, -1];
let grid = [
    [-1, -1, -1, -1, -1, -1, -1, -1],
    [-1, -1, -1, -1, -1, -1, -1, -1],
    [-1, -1, -1, -1, -1, -1, -1, -1],
    [-1, -1, -1, -1, -1, -1, -1, -1],
    [-1, -1, -1, -1, -1, -1, -1, -1],
    [-1, -1, -1, -1, -1, -1, -1, -1],
    [-1, -1, -1, -1, -1, -1, -1, -1],
    [-1, -1, -1, -1, -1, -1, -1, -1]
];
let bef_grid = [
    [-1, -1, -1, -1, -1, -1, -1, -1],
    [-1, -1, -1, -1, -1, -1, -1, -1],
    [-1, -1, -1, -1, -1, -1, -1, -1],
    [-1, -1, -1, -1, -1, -1, -1, -1],
    [-1, -1, -1, -1, -1, -1, -1, -1],
    [-1, -1, -1, -1, -1, -1, -1, -1],
    [-1, -1, -1, -1, -1, -1, -1, -1],
    [-1, -1, -1, -1, -1, -1, -1, -1]
];
var player = 0;
var ai_player = -1;
var tl = 50;
var tl_idx = -1;
let tl_names = ['レベル-2', 'レベル-1', 'レベル1', 'レベル2', 'レベル3', 'レベル4', 'レベル5', 'レベル6', 'レベル7'];
let record = [];
var step = 0;
var direction = -1;
var isstart = true;
var ctx = document.getElementById("graph");
var graph = new Chart(ctx, {
    type: 'line',
    data: {
    labels: [],
    datasets: [
        {
        label: "Prediction of stone difference",
        data: [],
        fill: false,
        borderColor: "rgb(0,0,0)",
        backgroundColor: "rgb(0,0,0)"
        }
    ],
    },
    options: {
        title: {
            display: false
        },
        legend: {
            display: false
        },
        scales: {
            yAxes: [{
            ticks: {
                suggestedMax: 5.0,
                suggestedMin: -5.0,
                stepSize: 5.0,
                callback: function(value, index, values){
                    return  value
                }
            }
            }]
        },
    }
});

function start() {
    ai_player = -1;
    let players = document.getElementsByName('ai_player');
    for (var i = 0; i < 2; ++i) {
        players.item(i).disabled = true;
        if (players.item(i).checked) {
            ai_player = players.item(i).value;
        }
    }
    let tls = document.getElementsByName('tl');
    var ln = tls.length;
    for (var i = 0; i < ln; ++i) {
        tls.item(i).disabled = true;
        if (tls.item(i).checked) {
            tl = tls.item(i).value;
            tl_idx = i;
        }
    }
    for (var yy = 0; yy < hw; ++yy) {
        for (var xx = 0; xx < hw; ++xx) {
            bef_grid[yy][xx] = grid[yy][xx];
        }
    }
    document.getElementById('start').disabled = true;
    var data_json = {};
    data_json["ai_player"] = ai_player;
    $.ajax({
        type: "POST",
        url: "/start",
        data: data_json,
        async: false,
        dataType: "json",
    }).done(function(data) {
        show(-1, -1);
    }).fail(function(data) {
        alert("[ERROR] cannot start click OK to try again");
        setTimeout(start(), "300");
    });
}

function show(r, c) {
    var table = document.getElementById("board");
    if (!check_mobility()) {
        player = 1 - player;
        if (!check_mobility()) {
            player = 2;
        }
    }
    for (var y = 0; y < 8; ++y) {
        for (var x = 0; x < 8; ++x) {
            table.rows[y].cells[x].style.backgroundColor = "#249972";
            if (grid[y][x] == 0) {
                if (bef_grid[y][x] != 0) {
                    table.rows[y].cells[x].innerHTML = '<span class="black_stone"></span>';
                    //table.rows[y].cells[x].firstChild.className ="black_stone";
                    table.rows[y].cells[x].setAttribute('onclick', "");
                }
            } else if (grid[y][x] == 1) {
                if (bef_grid[y][x] != 1) {
                    table.rows[y].cells[x].innerHTML = '<span class="white_stone"></span>';
                    //table.rows[y].cells[x].firstChild.className ="white_stone";
                    table.rows[y].cells[x].setAttribute('onclick', "");
                }
            } else if (grid[y][x] == 2) {
                if (r == -1 || inside(r, c)) {
                    //table.rows[y].cells[x].innerHTML = '<span class="legal_stone"></span>';
                    table.rows[y].cells[x].firstChild.className ="legal_stone";
                    table.rows[y].cells[x].setAttribute('onclick', "move(this.parentNode.rowIndex, this.cellIndex)");
                } else {
                    //table.rows[y].cells[x].innerHTML = '<span class="empty_stone"></span>';
                    table.rows[y].cells[x].firstChild.className ="empty_stone";
                    table.rows[y].cells[x].setAttribute('onclick', "");
                }
            } else {
                //table.rows[y].cells[x].innerHTML = '<span class="empty_stone"></span>';
                table.rows[y].cells[x].firstChild.className ="empty_stone";
                table.rows[y].cells[x].setAttribute('onclick', "");
            }
        }
    }
    if (inside(r, c)) {
        table.rows[r].cells[c].style.backgroundColor = "#d14141";
    }
    var black_count = 0, white_count = 0;
    for (var y = 0; y < hw; ++y) {
        for (var x = 0; x < hw; ++x) {
            if (grid[y][x] == 0)
                ++black_count;
            else if (grid[y][x] == 1)
                ++white_count;
        }
    }
    table = document.getElementById("status");
    table.rows[0].cells[2].firstChild.innerHTML = black_count;
    table.rows[0].cells[4].firstChild.innerHTML = white_count;
    if (player == 0) {
        table.rows[0].cells[0].firstChild.className = "legal_stone";
        table.rows[0].cells[6].firstChild.className = "state_blank";
    } else if (player == 1) {
        table.rows[0].cells[0].firstChild.className = "state_blank";
        table.rows[0].cells[6].firstChild.className = "legal_stone";
    } else {
        table.rows[0].cells[0].firstChild.className = "state_blank";
        table.rows[0].cells[6].firstChild.className = "state_blank";
    }
    if (player == ai_player){
        ai();
    }
}

function draw(element){
    if (!element) { return; }
    var n = document.createTextNode(' ');
    var disp = element.style.display;
    element.appendChild(n);
    element.style.display = 'none';
    setTimeout(function(){
        element.style.display = disp;
        n.parentNode.removeChild(n);
    },20);
}

function empty(y, x) {
    return grid[y][x] == -1 || grid[y][x] == 2;
}

function inside(y, x) {
    return 0 <= y && y < hw && 0 <= x && x < hw;
}

function check_mobility() {
    var res = false;
    for (var y = 0; y < hw; ++y) {
        for (var x = 0; x < hw; ++x) {
            if (!empty(y, x))
                continue;
            grid[y][x] = -1;
            for (var dr = 0; dr < 8; ++dr) {
                var ny = y + dy[dr];
                var nx = x + dx[dr];
                if (!inside(ny, nx))
                    continue;
                if (empty(ny, nx))
                    continue;
                if (grid[ny][nx] == player)
                    continue;
                var flag = false;
                var nny = ny, nnx = nx;
                for (var d = 0; d < hw; ++d) {
                    if (!inside(nny, nnx))
                        break;
                    if (empty(nny, nnx))
                        break;
                    if (grid[nny][nnx] == player) {
                        flag = true;
                        break;
                    }
                    nny += dy[dr];
                    nnx += dx[dr];
                }
                if (flag) {
                    grid[y][x] = 2;
                    res = true;
                    break;
                }
            }
        }
    }
    return res;
}

window.onload = function init() {
    grid[3][3] = 1
    grid[3][4] = 0
    grid[4][3] = 0
    grid[4][4] = 1
    player = 0;
    var coord_top = document.getElementById('coord_top');
    var row = document.createElement('tr');
    for (var x = 0; x < hw; ++x) {
        var cell = document.createElement('td');
        cell.className = "coord_cell";
        var coord = document.createElement('span');
        coord.className = "coord";
        coord.innerHTML = String.fromCharCode(65 + x);
        cell.appendChild(coord);
        row.appendChild(cell);
    }
    coord_top.appendChild(row);
    var coord_left = document.getElementById('coord_left');
    for (var y = 0; y < hw; ++y) {
        var row = document.createElement('tr');
        var cell = document.createElement('td');
        cell.className = "coord_cell";
        var coord = document.createElement('span');
        coord.className = "coord";
        coord.innerHTML = y + 1;
        cell.appendChild(coord);
        row.appendChild(cell);
        coord_left.appendChild(row);
    }
    var coord_right = document.getElementById('coord_right');
    for (var y = 0; y < hw; ++y) {
        var row = document.createElement('tr');
        var cell = document.createElement('td');
        cell.className = "coord_cell";
        var coord = document.createElement('span');
        coord.className = "coord";
        cell.appendChild(coord);
        row.appendChild(cell);
        coord_right.appendChild(row);
    }
    var table = document.getElementById('board');
    for (var y = 0; y < hw; ++y) {
        var row = document.createElement('tr');
        for (var x = 0; x < hw; ++x) {
            var cell = document.createElement('td');
            cell.className = "cell";
            var stone = document.createElement('span');
            stone.className = "empty_stone";
            cell.appendChild(stone);
            row.appendChild(cell);
        }
        table.appendChild(row);
    }
    show(-2, -2);
}

function ai() {
    var data_json = {};
    for (var y = 0; y < hw; ++y) {
        for (var x = 0; x < hw; ++x) {
            data_json[y * hw + x] = grid[y][x];
        }
    }
    $.ajax({
        type: "POST",
        url: "/ai",
        data: data_json,
        async: false,
        dataType: "json",
    }).done(function(data) {
        const received_data = JSON.parse(data.values);
        var r = received_data["r"];
        var c = received_data["c"];
        var s = received_data["s"];
        update_graph(s);
        if (r < 0 || 8 <= r || c < 0 || 8 <= c) {
            if (r == -1) {
                alert("[ERROR] grid broken status -1");
            } else if (r == -2) {
                alert("[ERROR] grid broken status -2");
            } else if (r == -3) {
                alert("[ERROR] ai_player broken status -3");
            } else if (r == -4) {
                alert("[ERROR] ai_player broken status -4");
            } else if (r == -5) {
                alert("[ERROR] time limit out of range status -5");
            } else if (r == -6) {
                alert("[ERROR] time limit broken status -6");
            }
            setTimeout(ai(), "300");
        } else {
            move(r, c);
            console.log("done");
        }
    }).fail(function(data) {
        alert("[ERROR] connection failed click OK to try again");
        setTimeout(ai(), "300");
    });
}

function move(y, x) {
    if (isstart) {
        isstart = false;
        if (x == 5)
            direction = 0;
        else if (x == 4)
            direction = 1;
        else if (x == 3)
            direction = 3;
        else
            direction = 2;
    }
    for (var yy = 0; yy < hw; ++yy) {
        for (var xx = 0; xx < hw; ++xx) {
            bef_grid[yy][xx] = grid[yy][xx];
        }
    }
    grid[y][x] = player;
    for (var dr = 0; dr < 8; ++dr) {
        var ny = y + dy[dr];
        var nx = x + dx[dr];
        if (!inside(ny, nx))
            continue;
        if (empty(ny, nx))
            continue;
        if (grid[ny][nx] == player)
            continue;
        var flag = false;
        var nny = ny, nnx = nx;
        var plus = 0;
        for (var d = 0; d < hw; ++d) {
            if (!inside(nny, nnx))
                break;
            if (empty(nny, nnx))
                break;
            if (grid[nny][nnx] == player) {
                flag = true;
                break;
            }
            nny += dy[dr];
            nnx += dx[dr];
            ++plus;
        }
        if (flag) {
            for (var d = 0; d < plus; ++d) {
                grid[ny + d * dy[dr]][nx + d * dx[dr]] = player;
            }
        }
    }
    ++record.length;
    record[record.length - 1] = [y, x];
    update_record();
    player = 1 - player;
    show(y, x);
}

function update_record() {
    var record_html = document.getElementById('record');
    var new_coord = String.fromCharCode(65 + record[record.length - 1][1]) + String.fromCharCode(49 + record[record.length - 1][0]);
    record_html.innerHTML += new_coord;
}

function update_graph(s) {
    graph.data.labels.push(record.length);
    graph.data.datasets[0].data.push(s);
    graph.update();
}
