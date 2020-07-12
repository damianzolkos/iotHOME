const configAddress = "config.json";

let notifications = 0;
let messages = [];
let config;

async function getData(url) {
    const data = await fetch(url)
        .then(
            function (response) {
                if (response.status !== 200) {
                    console.log('There was a problem. Status Code: ' +
                        response.status);
                    return "err";
                }
                return response.json();
            }
        )
        .catch(function (err) {
            console.log('Fetch Error', err);
        });
    return data;
}

var mq = window.matchMedia("(max-width: 500px)");

async function onLoad() {
    config = await getData(configAddress);

    console.log("");
    console.log("%c ###############################", "font-weight: bold;");
    console.log("            " + config.appName);
    console.log("%c ###############################", "font-weight: bold;");
    console.log("");

    window.document.title = config.appName;
    document.getElementById("logo").innerHTML = config.appName;
    document.getElementById("mobile_logo").innerHTML = config.appName;
    document.getElementById("author").innerHTML = config.appAuthor;
    document.getElementById("authorLink").href = config.appAuthorLink;

    if (mq.matches == true) {
        showOrHide = 1;
    } else {
        showOrHide = 0;
    }

    clock();
    notificationsCounterUpdate();

    // show
    // alarm(config.appName, "Simple and responsive web app interface with modules support.", "img/favicon.png");
    // newNotification("Test notification #1");
    // newNotification("Test notification #2");

    config.pages.forEach((item) => {
        createSidebarMenuItem(item.name);
        createPages(item);
    });
    changeScreen(config.pages[0].name);


    config.activeModules.forEach((item) => {
        let module_name = item[0];
        let module_page;

        if (typeof item[1] == "string") {

            $.getJSON("modules/" + module_name + "/" + module_name + ".json", function (json) {
                let module = JSON.parse(JSON.stringify(json));
    
                if (module.visibility == "none") {
                    module_page = config.pages[0];
                } else {
                    if (item[1] == undefined) {
                        module_page = module.defaultPage;
                        if (config.pages.includes(module.defaultPage) == false) {
                            config.pages.push(module.defaultPage);
                            createSidebarMenuItem(module.defaultPage);
                            createPages(module.defaultPage);
                            changeScreen(config.pages[0]);
                        }
                    } else module_page = item[1];
                }
    
                if (createBox(module_page, module.id + Math.floor(Math.random() * 1000000000), module.name, module.visibility)) {
                    console.log("loaded: " + module_name);
                }
            });
        } else console.log("new module config, not yet supported.");
    });
    console.log("");
}

function clock() {
    getTime();
    setTimeout(clock, 1000);
}

function getTime() {
    var month = new Array();
    month[0] = "January";
    month[1] = "February";
    month[2] = "March";
    month[3] = "April";
    month[4] = "May";
    month[5] = "June";
    month[6] = "July";
    month[7] = "August";
    month[8] = "September";
    month[9] = "October";
    month[10] = "November";
    month[11] = "December";

    var d = new Date();
    var month = month[d.getMonth()];
    var year = d.getFullYear();
    var day = d.getDate();
    var hour = d.getHours();
    var minutes = d.getMinutes();

    if (minutes == 0) minutes = "00";
    else if (minutes == 1) minutes = "01";
    else if (minutes == 2) minutes = "02";
    else if (minutes == 3) minutes = "03";
    else if (minutes == 4) minutes = "04";
    else if (minutes == 5) minutes = "05";
    else if (minutes == 6) minutes = "06";
    else if (minutes == 7) minutes = "07";
    else if (minutes == 8) minutes = "08";
    else if (minutes == 9) minutes = "09";

    h = hour.toString();
    m = minutes.toString();
    mo = month;
    da = day.toString();
    y = year.toString();
}

function alarm(title, message, icon) {
    $('body').addClass('stop-scrolling');
    document.getElementById('shade').style.display = "flex";
    document.getElementById('popup').style.display = "block";

    document.getElementById('popup_icon').src = icon;
    document.getElementById('popup_title').innerHTML = title;
    document.getElementById('popup_message').innerHTML = message;
}

function hideAlert() {
    $('body').removeClass('stop-scrolling');
    document.getElementById('shade').style.display = "none";
    document.getElementById('popup').style.display = "none";
}

function changeScreen(nameOfScreen) {
    config.pages.forEach((item) => {
        document.getElementById(item.name + "_menuItem").className = "inactive";
        document.getElementById(item.name).style.display = "none";
    });
    document.getElementById(nameOfScreen + "_menuItem").className = "active";
    document.getElementById(nameOfScreen).style.display = "";

    mq = window.matchMedia("(max-width: 500px)");
    if (mq.matches == true) {
        showSidebar();
    }
}

function createPages(page) {
    let newPage = document.createElement("div");
    newPage.id = page.name;
    newPage.className = "page "+page.type;
    document.getElementById("wrapper").appendChild(newPage);
}

function createSidebarMenuItem(nameOfMenuItem) {
    let menuElement = document.createElement("li");
    menuElement.innerHTML = nameOfMenuItem;
    menuElement.setAttribute("onclick", "changeScreen('" + nameOfMenuItem + "');");
    menuElement.id = nameOfMenuItem + "_menuItem";
    menuElement.style.listStyleType = 'none';
    document.getElementById("sidebarMenu").appendChild(menuElement);
}

function createBox(pageName, boxId, moduleName, moduleVisibility) {
    let box = document.createElement("div");
    box.className = "box";
    box.id = boxId;
    document.getElementById(pageName).appendChild(box);
    let moduleNameId = "#" + boxId;
    let moduleFile = "modules/" + moduleName + "/" + moduleName + ".html";
    $(moduleNameId).load(moduleFile);
    if (moduleVisibility == "none") {
        box.style.display = "none";
    } else if (moduleVisibility == "visible") {
        box.style.display = "inline-block";
    }
    return true;
}

function notificationsWindowToggle() {
    let notificationsWindow = document.getElementById('notifications');
    let windowState = notificationsWindow.style.visibility;
    if (windowState === "hidden" || windowState === "") {
        notificationsWindow.style.visibility = "visible";
        notificationsWindow.style.opacity = 1;
    } else {
        notificationsWindow.style.visibility = "hidden";
        notificationsWindow.style.opacity = 0;
    }
}

function notificationsCounterUpdate() {
    let notification_counter = document.getElementById('notifications_counter');
    let noNotifications = document.getElementById('noNotifications');
    let clearNotifications = document.getElementById('clear_notifications');

    if (notifications == 0) {
        notification_counter.style.visibility = "hidden";
        noNotifications.style.display = "block";
        clearNotifications.style.visibility = "hidden";
    } else {
        notification_counter.style.visibility = "visible";
        clearNotifications.style.visibility = "visible";
        noNotifications.style.display = "none";
    }
    notification_counter.innerHTML = notifications;
}

function newNotification(message) {
    messages[notifications] = message;
    notifications++;
    let newMessageLi = document.createElement("li");
    newMessageLi.innerHTML = h + ":" + m + " - " + message;
    document.getElementById("notifications_list").appendChild(newMessageLi);
    notificationsCounterUpdate();
}

function clearNotifications() {
    notifications = 0;
    while (messages.length > 0) {
        messages.pop();
    }
    $("#notifications_list").children(":not(#noNotifications)").remove();
    notificationsCounterUpdate();
    notificationsWindowToggle();
}

function showSidebar() {
    let sidebar = document.getElementById('sidebar');
    let content = document.getElementById('content');
    if (showOrHide == 0) {
        sidebar.style.transform = 'translate(0px,0px)';
        content.style.left = "250px";

        if (mq.matches == false) {
            content.style.width = 'calc(100% - 250px)';
        }

        showOrHide = 1;
        document.getElementById("mobile_logo").style.display = "none";
    } else if (showOrHide == 1) {
        sidebar.style.transform = 'translate(-250px,0px)';
        content.style.left = "0px";
        content.style.width = '100%';
        showOrHide = 0;
        document.getElementById("mobile_logo").style.display = "inline-block";
    }
}


// $('body').removeClass('stop-scrolling');
// $('body').addClass('stop-scrolling');