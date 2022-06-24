#include "DatabaseAccess.h"

sqlite3* DatabaseAccess::db = nullptr;

//opens db
bool DatabaseAccess::open()
{
	string dbFileName = "GalleryArad.sqlite";
	int doesFileExist = _access(dbFileName.c_str(), 0);
	int res = sqlite3_open(dbFileName.c_str(), &db);
	if (res != SQLITE_OK)
	{
		db = nullptr;
		return false;
	}
	if (doesFileExist == 0)
	{
		const char* sqlStatement = "CREATE TABLE IF NOT EXISTS USERS (ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, NAME TEXT NOT NULL);";
		char** errMessage = nullptr;
		res = sqlite3_exec(db, sqlStatement, nullptr, nullptr, errMessage);
		if (res != SQLITE_OK)
			return false;

		sqlStatement = "CREATE TABLE IF NOT EXISTS ALBUMS(ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, NAME TEXT, CREATION_DATE TEXT, USER_ID INT, FOREIGN KEY(USER_ID) REFERENCES USERS(ID));";
		errMessage = nullptr;
		res = sqlite3_exec(db, sqlStatement, nullptr, nullptr, errMessage);
		if (res != SQLITE_OK)
			return false;

		sqlStatement = "CREATE TABLE IF NOT EXISTS PICTURES(ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, NAME TEXT,CREATION_DATE TEXT, LOCATION TEXT, ALBUM_ID INT, FOREIGN KEY(ALBUM_ID) REFERENCES ALBUMS(ID));";
		errMessage = nullptr;
		res = sqlite3_exec(db, sqlStatement, nullptr, nullptr, errMessage);
		if (res != SQLITE_OK)
			return false;

		sqlStatement = "CREATE TABLE IF NOT EXISTS TAGS(ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, PICTURE_ID INT, USER_ID INT, FOREIGN KEY(USER_ID) REFERENCES USERS(ID), FOREIGN KEY(PICTURE_ID) REFERENCES PICTURES(ID));";
		errMessage = nullptr;
		res = sqlite3_exec(db, sqlStatement, nullptr, nullptr, errMessage);
		if (res != SQLITE_OK)
			return false;
	}
	return true;
}
//closes db
void DatabaseAccess::close()
{
	sqlite3_close(db);
	db = nullptr;
}
//clears(?)
void DatabaseAccess::clear()
{
}
//returns list of albums that belong to user
const std::list<Album> DatabaseAccess::getAlbumsOfUser(const User& user)
{
	string msg = "SELECT * FROM ALBUMS WHERE USER_ID=" + to_string(user.getId()) + ";";
	const char* sqlStatement = msg.c_str();
	char** errMessage = nullptr;
	list<Album> albums;
	int res = sqlite3_exec(db, sqlStatement, callbackAlb, &albums, errMessage);
	return albums;
}
//returnes list of all albums
const std::list<Album> DatabaseAccess::getAlbums()
{
	string msg = "SELECT * FROM ALBUMS;";
	const char* sqlStatement = msg.c_str();
	char** errMessage = nullptr;
	list<Album> albums;
	int res = sqlite3_exec(db, sqlStatement, callbackAlb, &albums, errMessage);
	return albums;
}
//creates new album
void DatabaseAccess::createAlbum(const Album& album)
{
	string msg = "INSERT INTO ALBUMS (NAME,CREATION_DATE,USER_ID) VALUES (\"" + album.getName() + "\",\"" + album.getCreationDate() + "\"," + to_string(album.getOwnerId()) + ");";
	cout << msg;
	const char* sqlStatement = msg.c_str();
	char** errMessage = nullptr;
	int res = sqlite3_exec(db, sqlStatement, nullptr, nullptr, errMessage);
}
//closes album
void DatabaseAccess::closeAlbum(Album& pAlbum)
{
	
}
//returns album
Album DatabaseAccess::openAlbum(const std::string& albumName)
{
	string msg = "SELECT * FROM ALBUMS WHERE NAME=\"" + albumName + "\";";
	const char* sqlStatement = msg.c_str();
	char** errMessage = nullptr;
	list<Album> albums;
	int res = sqlite3_exec(db, sqlStatement, callbackAlb, &albums, errMessage);
	return *albums.begin();
}
//prints list of albums
void DatabaseAccess::printAlbums()
{
	string msg = "SELECT * FROM ALBUMS;";
	const char* sqlStatement = msg.c_str();
	char* errMessage = nullptr;
	int res = sqlite3_exec(db, sqlStatement, callbackPrints, nullptr, &errMessage);
}
//creates user
void DatabaseAccess::createUser(User& user)
{
	string msg = "INSERT INTO USERS (ID, NAME) VALUES ("+to_string(user.getId())+",\"" + user.getName() + "\");";
	const char* sqlStatement = msg.c_str();
	char** errMessage = nullptr;
	int res = sqlite3_exec(db, sqlStatement, nullptr, nullptr, errMessage);
}
//tags in picture
void DatabaseAccess::tagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId)
{
	string msg = "INSERT INTO TAGS (USER_ID,PICTURE_ID) SELECT " + to_string(userId) + ", PICTURES.ID FROM PICTURES WHERE PICTURES.NAME=\""+pictureName+"\";";
	const char* sqlStatement = msg.c_str();
	char** errMessage = nullptr;
	int res = sqlite3_exec(db, sqlStatement, nullptr, nullptr, errMessage);
}
//yntags in picture
void DatabaseAccess::untagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId)
{
	string msg = "DELETE FROM TAGS WHERE PICTURE_ID=(SELECT PICTURES.ID FROM PICTURES WHERE PICTURES.NAME=\"" + pictureName +"\") AND TAGS.USER_ID=" + to_string(userId)+";";
	const char* sqlStatement = msg.c_str();
	char** errMessage = nullptr;
	int res = sqlite3_exec(db, sqlStatement, nullptr, nullptr, errMessage);
}
//deletes album
void DatabaseAccess::deleteAlbum(const std::string& albumName, int userId)
{
	//deletes all pictures from table
	string msg = "DELETE FROM PICTURES WHERE ALBUM_ID = (SELECT ALBUMS.ID FROM ALBUMS WHERE ALBUMS.NAME = \"" + albumName + "\" AND ALBUMS.USER_ID=" + to_string(userId) + ");";
	const char* sqlStatement = msg.c_str();
	char** errMessage = nullptr;
	int res = sqlite3_exec(db, sqlStatement, nullptr, nullptr, errMessage);
	//delete all tags with picID that is missing in pictures table
	msg = "DELETE FROM TAGS WHERE PICTURE_ID NOT IN (SELECT ID FROM PICTURES);";
	sqlStatement = msg.c_str();
	errMessage = nullptr;
	res = sqlite3_exec(db, sqlStatement, nullptr, nullptr, errMessage);
	//delete album
	msg = "DELETE FROM ALBUMS WHERE NAME = \"" + albumName + "\" AND USER_ID = " + to_string(userId) + ";";
	sqlStatement = msg.c_str();
	errMessage = nullptr;
	res = sqlite3_exec(db, sqlStatement, nullptr, nullptr, errMessage);
}
//returns if album exists
bool DatabaseAccess::doesAlbumExists(const std::string& albumName, int userId)
{
	string msg = "SELECT * FROM ALBUMS WHERE NAME=\"" + albumName + "\" AND USER_ID=" + to_string(userId) + ";";
	const char* sqlStatement = msg.c_str();
	char** errMessage = nullptr;
	list<Album> albums;
	int res = sqlite3_exec(db, sqlStatement, callbackAlb, &albums, errMessage);
	return !albums.empty();
}
//deletes user
void DatabaseAccess::deleteUser(const User& user)
{
	//deletes from pictures with albumID that matches owner id in albums
	string msg = "DELETE FROM PICTURES WHERE ALBUM_ID = (SELECT ID FROM ALBUMS WHERE ALBUMS.USER_ID=" + to_string(user.getId()) + ");";
	const char* sqlStatement = msg.c_str();
	char** errMessage = nullptr;
	int res = sqlite3_exec(db, sqlStatement, nullptr, nullptr, errMessage);
	//deletes from tags with missing picID
	msg = "DELETE FROM TAGS WHERE PICTURE_ID NOT IN (SELECT ID FROM PICTURES);";
	sqlStatement = msg.c_str();
	errMessage = nullptr;
	res = sqlite3_exec(db, sqlStatement, nullptr, nullptr, errMessage);
	//deletes the user from all tags
	msg = "DELETE FROM TAGS WHERE USER_ID=" + to_string(user.getId()) + ";";
	sqlStatement = msg.c_str();
	errMessage = nullptr;
	res = sqlite3_exec(db, sqlStatement, nullptr, nullptr, errMessage);
	//deletes all albums that with the users id 
	msg = "DELETE FROM ALBUMS WHERE USER_ID=" + to_string(user.getId()) + ";";
	sqlStatement = msg.c_str();
	errMessage = nullptr;
	res = sqlite3_exec(db, sqlStatement, nullptr, nullptr, errMessage);
	//deletes user
	msg = "DELETE FROM USERS WHERE USER_ID=" + to_string(user.getId()) + ";";
	sqlStatement = msg.c_str();
	errMessage = nullptr;
	res = sqlite3_exec(db, sqlStatement, nullptr, nullptr, errMessage);
}
//prints user
void DatabaseAccess::printUsers()
{
	string msg = "SELECT * FROM USERS;";
	const char* sqlStatement = msg.c_str();
	char* errMessage = nullptr;
	int res = sqlite3_exec(db, sqlStatement, callbackPrints, nullptr, &errMessage);
}
//checks if user exists
bool DatabaseAccess::doesUserExists(int userId)
{
	string msg = "SELECT * FROM USERS WHERE ID=" + to_string(userId) + ";";
	const char* sqlStatement = msg.c_str();
	char** errMessage = nullptr;
	list<User> users;
	int res = sqlite3_exec(db, sqlStatement, callbackUser, &users, errMessage);
	return !users.empty();
}
//returns user
User DatabaseAccess::getUser(int userId)
{
	string msg = "SELECT * FROM USERS WHERE ID=" + to_string(userId) + ";";
	const char* sqlStatement = msg.c_str();
	char** errMessage = nullptr;
	list<User> users;
	int res = sqlite3_exec(db, sqlStatement, callbackUser, &users, errMessage);
	return *users.begin();
}
//adds picture
void DatabaseAccess::addPictureToAlbumByName(const std::string& albumName, const Picture& picture)
{
	string msg = "INSERT INTO PICTURES (NAME, CREATION_DATE, LOCATION, ALBUM_ID) SELECT \"" + picture.getName() + "\", \"" + picture.getCreationDate() + "\", \"" + picture.getPath() + "\", ALBUMS.ID FROM ALBUMS WHERE ALBUMS.NAME=\"" + albumName + "\";";
	const char* sqlStatement = msg.c_str();
	char** errMessage = nullptr;
	int res = sqlite3_exec(db, sqlStatement, nullptr, nullptr, errMessage);
}
//removes picture
void DatabaseAccess::removePictureFromAlbumByName(const std::string& albumName, const std::string& pictureName)
{
	//deletes tags with the same picID
	string msg = "DELETE FROM TAGS WHERE PICTURE_ID=(SELECT PICTURES.ID FROM PICTURES WHERE PICTURES.NAME=\""+pictureName+"\");";
	const char* sqlStatement = msg.c_str();
	char** errMessage = nullptr;
	int res = sqlite3_exec(db, sqlStatement, nullptr, nullptr, errMessage);
	//deletes the picture
	msg = "DELETE FROM PICTURES WHERE ALBUM_ID = (SELECT ALBUMS.ID FROM ALBUMS WHERE ALBUMS.NAME = \"" + albumName + "\") AND PICTURES.NAME=\"" + pictureName + "\";";
	sqlStatement = msg.c_str();
	errMessage = nullptr;
	res = sqlite3_exec(db, sqlStatement, nullptr, nullptr, errMessage);
}
//counts albums
int DatabaseAccess::countAlbumsTaggedOfUser(const User& user)
{
	string msg = "SELECT u.ID, u.NAME, COUNT(DISTINCT p.ALBUM_ID) counter FROM USERS u LEFT JOIN TAGS t ON t.USER_ID = u.ID LEFT JOIN PICTURES p ON p.ID = t.PICTURE_ID WHERE u.ID =" + to_string(user.getId()) + ";";
	const char* sqlStatement = msg.c_str();
	char** errMessage = nullptr;
	list<int> amounts;
	int res = sqlite3_exec(db, sqlStatement, callCounter, &amounts, errMessage);
	return *amounts.begin();
}
//calcs avg
float DatabaseAccess::averageTagsPerAlbumOfUser(const User& user)
{
	int albumsTaggedCount = countAlbumsTaggedOfUser(user);

	if (0 == albumsTaggedCount) {
		return 0;
	}

	return static_cast<float>(countTagsOfUser(user)) / albumsTaggedCount;
}
//counts amount of albums for user
int DatabaseAccess::countAlbumsOwnedOfUser(const User& user)
{
	string msg = "SELECT COUNT(USER_ID) AS Counter FROM ALBUMS WHERE USER_ID=" + to_string(user.getId()) + ";";
	const char* sqlStatement = msg.c_str();
	char** errMessage = nullptr;
	list<int> amounts;
	int res = sqlite3_exec(db, sqlStatement, callCounter, &amounts, errMessage);
	return *amounts.begin();
}
//counts amount of tags
int DatabaseAccess::countTagsOfUser(const User& user)
{
	string msg = "SELECT COUNT(USER_ID) FROM TAGS WHERE USER_ID=" + to_string(user.getId()) + ";";
	const char* sqlStatement = msg.c_str();
	char** errMessage = nullptr;
	list<int> amounts;
	int res = sqlite3_exec(db, sqlStatement, callCounter, &amounts, errMessage);
	return *amounts.begin();
}
//returns most tagged user
User DatabaseAccess::getTopTaggedUser()
{
	string msg = "SELECT * FROM USERS WHERE ID = (SELECT USER_ID FROM TAGS GROUP BY USER_ID ORDER BY COUNT(*) DESC LIMIT 1);";
	const char* sqlStatement = msg.c_str();
	char** errMessage = nullptr;
	list<User> users;
	int res = sqlite3_exec(db, sqlStatement, callbackUser, &users, errMessage);
	return *users.begin();
}
//returns most tagged picture
Picture DatabaseAccess::getTopTaggedPicture()
{
	string msg = "SELECT * FROM PICTURES WHERE ID = (SELECT PICTURE_ID FROM TAGS GROUP BY PICTURE_ID ORDER BY COUNT(*) DESC LIMIT 1);";
	const char* sqlStatement = msg.c_str();
	char** errMessage = nullptr;
	list<Picture> pics;
	int res = sqlite3_exec(db, sqlStatement, callbackPic, &pics, errMessage);
	return *pics.begin();
}
//returns list of pictures that the user is tagged at
std::list<Picture> DatabaseAccess::getTaggedPicturesOfUser(const User& user)
{
	string msg = "SELECT * FROM TAGS WHERE USER_ID=" + to_string(user.getId()) + ";";
	const char* sqlStatement = msg.c_str();
	char** errMessage = nullptr;
	list<Picture> pics;
	int res = sqlite3_exec(db, sqlStatement, DatabaseAccess::callbackPic, &pics, errMessage);
	return pics;
}
//prints from query 
int DatabaseAccess::callbackPrints(void* data, int argc, char** argv, char** azColName)
{
	for (int i = 0; i < argc; i++)
	{
		cout << azColName[i] << " = " << argv[i] << " , ";
	}
	cout << endl;
	return 0;
}
//when the func finishes the data var will have a list of users
int DatabaseAccess::callbackUser(void* data, int argc, char** argv, char** azColName)
{
	int userid = 0;
	string userName = "";
	for (int i = 0; i < argc; i++)
	{
		if(string(azColName[i])=="ID")
			userid = atoi(argv[i]);
		else if(string(azColName[i]) == "NAME")
			userName = argv[i];
	}
	list<User>* users = (list<User>*)data;
	users->push_back(User(userid, userName));
	return 0;
}
//when the func finishes the data var will have a list of pictures
int DatabaseAccess::callbackPic(void* data, int argc, char** argv, char** azColName)
{
	int picid = 0;
	string picname = "";
	string create = "";
	string loc = "";
	for (int i = 0; i < argc; i++)
	{
		if (string(azColName[i]) == "ID")
			picid = atoi(argv[i]);
		else if (string(azColName[i]) == "NAME")
			picname = argv[i];
		else if (string(azColName[i]) == "LOCATION")
			loc = argv[i];
		else if (string(azColName[i]) == "CREATION_DATE")
			create = argv[i];
	}
	//gets a list of all tags that belong to this picture
	string msg = "SELECT USER_ID AS Counter FROM TAGS WHERE PICTURE_ID=" + to_string(picid) + ";";
	const char* sqlStatement = msg.c_str();
	char** errMessage = nullptr;
	list<int> tags;
	int res = sqlite3_exec(db, sqlStatement, DatabaseAccess::callCounter, &tags, errMessage);

	list<Picture>* pic = (list<Picture>*)data;
	Picture* curr = new Picture(picid, picname, loc, create);
	for (int i = 0; i < tags.size(); i++)
	{
		list<int>::iterator iter = tags.begin();
		advance(iter, i);
		curr->tagUser(*iter);
	}
	pic->push_back(*curr);
	return 0;
}
//when the func finishes the data var will have list of albums
int DatabaseAccess::callbackAlb(void* data, int argc, char** argv, char** azColName)
{
	int ownerid = 0;
	string albumName = "";
	string create = "";
	for (int i = 0; i < argc; i++)
	{
		if (string(azColName[i]) == "USER_ID")
			ownerid = atoi(argv[i]);
		else if (string(azColName[i]) == "NAME")
			albumName = argv[i];
		else if (string(azColName[i]) == "CREATION_DATE")
			create = argv[i];
	}
	list<Album>* albums = (list<Album>*)data;
	Album* curr = new Album(ownerid, albumName, create);
	//returns list of all pictures that belong to this album
	string msg = "SELECT * FROM PICTURES WHERE ALBUM_ID=(SELECT ID FROM ALBUMS WHERE USER_ID=" + to_string(ownerid) + ");";
	const char* sqlStatement = msg.c_str();
	char** errMessage = nullptr;
	list<Picture> pics;
	int res = sqlite3_exec(db, sqlStatement, DatabaseAccess::callbackPic, &pics, errMessage);

	for (int i = 0; i < pics.size(); i++)
	{
		list<Picture>::iterator iter = pics.begin();
		advance(iter, i);
		curr->addPicture(*iter);
	}

	albums->push_back(*curr);
	return 0;
}
//when the func finishes that data var will have a list of ints
int DatabaseAccess::callCounter(void* data, int argc, char** argv, char** azColName)
{
	int amount = 0;
	for (int i = 0; i < argc; i++)
	{
		if (string(azColName[i]) == "Counter")
			amount = atoi(argv[i]);
	}
	list<int>* amounts = (list<int>*)data;
	amounts->push_back(amount);
	return 0;
}
