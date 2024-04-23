#include "usermanager.h"
#define PATH "/Users/k80l80d/Desktop/GraphicsGameData"


UserManager::UserManager(QObject *parent) : QObject(parent) {
     qDebug() << "UserManager object created";
    deserialize();
}


UserManager::~UserManager() {
    qDebug() << "UserManager object destroyed";
}

//handles user authentication on login page
void UserManager::authenticateUser(QString userName, QString password) {
    // Perform login logic here
    qDebug() << "user manager: Attempting to log in with username:" << userName << "and password:" << password;
    // Dummy logic for demonstration
    print();
    //username and password matches
    if(usernameNuserInfo.contains(userName) && password == usernameNuserInfo.value(userName)->password){
       qDebug() << "usermanager: Login successful!";

       //snatch user data for specific id from usersInfoMap
       UserInfo* userInfo = usernameNuserInfo.value(userName);

       emit userAuthenticated(userInfo);
    }
    //username or password incorrect! Send signal to sign in form
    else{
        emit userAuthenticationFailed();
    }
}

//handles user sign up requests, upon signal sent from signup form
void UserManager::signUpUser(UserRequest* userRequest) {
    qDebug() << "UserManager: signal recieved! User wants to sign up ";
    qDebug() << "username: " << userRequest->userName;
    qDebug() << "password:" << userRequest->password;


    QString requestedUserName =  userRequest->userName;

    //TODO: KT after done testing, add check that no values inside userRequest are null?

    //username is taken, send signal to signup page
    if(usernameNuserInfo.contains(requestedUserName)){
        //emit signal
        emit userNameTaken();
        qDebug() << "UserManager: user name is taken signal sent";

    }
    //user signed up
    else{
        //add new users to "database"
        qDebug() << "UserManager: new user: " << requestedUserName;

        //transfer only data needed to render profile,
        UserInfo* userInfo = new UserInfo(this, userRequest);
        UserInfo::print(userInfo);
        usernameNuserInfo.insert(requestedUserName, userInfo);
        serialize();
        //signal sent to login page to render profile page
        emit userSignedUp(userInfo);
    }
}

void UserInfo::addScore(int score) {
    scoreHistory.append(score);
    UserManager* userManager = (UserManager*)manager;
    userManager->serialize();
}

 void UserInfo::print(const UserInfo* userInfo) {
    qDebug() << "Username:" << userInfo->username;
    qDebug() << "First Name:" << userInfo->firstName;
    qDebug() << "Last Name:" << userInfo->lastName;
    qDebug() << "Date of Birth:" << userInfo->dateOfBirth.toString("yyyy-MM-dd");
    qDebug() << "Score History:";
    for (int score : userInfo->scoreHistory) {
        qDebug() << score;
    }
    // Additional information about profile picture if needed
    //qDebug() << "Profile Picture:" << userInfo->profilePicture.isNull() ? "Not set" : "Set";
}

 void UserManager::print() {
     qDebug() << "printing user manager data";
     for (UserInfo* user : usernameNuserInfo) {
         UserInfo::print(user);
    }
 }

void UserManager::serialize() {
    QJsonObject userMap = QJsonObject();
    for (UserInfo* user : usernameNuserInfo) {
        QJsonObject userInfoJson;
        userInfoJson["username"] = user->username;
        userInfoJson["firstname"] = user->firstName;
        userInfoJson["lastname"] = user->lastName;
        userInfoJson["password"] = user->password;
        userInfoJson["dob"] = user->dateOfBirth.toString();
        bool picExists = !user->profilePicture.isNull();
        userInfoJson["picture"] = picExists;
        if (picExists) {
            QFile picFile(QString(PATH) + QString("/") + QString(user->username) + QString(".png"));
            if (!picFile.open(QFile::WriteOnly | QIODevice::Text)) {
                if (!picFile.open(QFile::WriteOnly | QIODevice::Text | QIODevice::NewOnly)) {
                    qDebug() << "Failed to open or create file for writing.";
                    continue;
                }
            }
            user->profilePicture.save(&picFile, "PNG");
            qDebug() << "wrote profile picture to file";
        }
        QJsonArray scoresJson = QJsonArray();
        for (int score : user->scoreHistory) {
            scoresJson.append(score);
        }
        userInfoJson["scores"] = scoresJson;
        userMap[user->username] = userInfoJson;
    }
    QJsonDocument jsonDoc = QJsonDocument(userMap);
    QFile file(QString(PATH) + QString("/userMapping.json"));
    if (!file.open(QFile::WriteOnly | QIODevice::Text)) {
        if (!file.open(QFile::WriteOnly | QIODevice::Text | QIODevice::NewOnly)) {
            qDebug() << "Failed to open or create file for writing.";
        }
        return;
    }
    file.write(jsonDoc.toJson());
    file.close();
    qDebug() << "User data written to file";
}

void UserManager::deserialize() {
    QFile file(QString(PATH) + QString("/userMapping.json"));
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file for reading.";
        return;
    }

    // Read the entire file content
    QByteArray fileData = file.readAll();
    file.close();

    // Parse the JSON data into a QJsonDocument
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(fileData, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "Failed to parse JSON:" << parseError.errorString();
    }

    // If the JSON document is an array, create a new one
    if (!jsonDoc.isObject()) {
        qDebug() << "JSON document is not an object.";
        jsonDoc = QJsonDocument(QJsonObject());
    }

    // Get the JSON array
    QJsonObject jsonObj = jsonDoc.object();

    usernameNuserInfo.clear();

    for (QString key : jsonObj.keys()) {
        QJsonValue userJson = jsonObj.value(key);
        if (!userJson.isObject()) {
            qDebug() << "JSON document is improperly formmatted";
            continue;
        }
        QJsonObject userJsonObj = userJson.toObject();
        if (!userJsonObj.keys().contains("username") ||
            !userJsonObj.keys().contains("firstname") ||
            !userJsonObj.keys().contains("lastname") ||
            !userJsonObj.keys().contains("password") ||
            !userJsonObj.keys().contains("dob") ||
            !userJsonObj.keys().contains("scores") ||
            !userJsonObj.keys().contains("picture")) {
            qDebug() << "JSON user info missing fields";
            continue;
        }
        QJsonValue usernameJson = userJsonObj.value("username");
        QJsonValue firstNameJson = userJsonObj.value("firstname");
        QJsonValue lastNameJson = userJsonObj.value("lastname");
        QJsonValue passwordJson = userJsonObj.value("password");
        QJsonValue dobJson = userJsonObj.value("dob");
        QJsonValue scoresJson = userJsonObj.value("scores");
        QJsonValue pictureJson = userJsonObj.value("picture");
        if (!usernameJson.isString() ||
            !firstNameJson.isString() ||
            !lastNameJson.isString() ||
            !passwordJson.isString() ||
            !dobJson.isString() ||
            !scoresJson.isArray() ||
            !pictureJson.isBool()) {
            qDebug() << "JSON user info fields invalid";
            continue;
        }
        QString username = usernameJson.toString();
        QString firstName = firstNameJson.toString();
        QString lastName = lastNameJson.toString();
        QString password = passwordJson.toString();
        QDate dob = QDate::fromString(dobJson.toString());
        bool picExists = pictureJson.toBool();
        QPixmap picture;
        if (picExists) {
            picture = QPixmap(QString(PATH) + QString("/") + username + QString(".png"));
        } else {
            picture = QPixmap();
        }
        QJsonArray scoresJsonArr = scoresJson.toArray();
        QVector<int> scores = QVector<int>();
        for (QJsonValue scoreJson : scoresJsonArr) {
            if (!scoreJson.isDouble()) {
                qDebug() << "JSON score improperly formatted";
                continue;
            }
            int score = scoreJson.toInt();
            scores.append(score);
        }
        UserInfo* userInfo = new UserInfo(this, username, firstName, lastName, password, dob, scores, picture);
        usernameNuserInfo.insert(username, userInfo);
    }
    qDebug() << "user data read from file";
}

