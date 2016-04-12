#include <iostream>
#include <string>
#include <assert.h>
#include <vector>
#include <boost/bind.hpp>
#include "ZooKeeper.h"

using namespace std;

void testSet(const char *path, const char *data);
void testCreate(const char *path, const char *data, char type, bool recursive);

void dataCallback(const std::string &path, const std::string &value)
{
	cout << "data changed: " << " path=" << path << ", data=" << value << endl;
}

void childrenCallback(const std::string &path, const vector<string> &children)
{
	cout << "children changed: " << ", path=" << path << ", children=";
	for(int i = 0; i < children.size(); ++i)
	{
		if(i != 0)
			cout << ",";
		cout << children[i];
	}
	cout << endl;
}

int main()
{
	ZooKeeper &zk = ZooKeeper::instance(); 
	if(!zk.init("127.0.0.1:2181,127.0.0.1:2182,127.0.0.1:2183"))
	{
		cout << "init zk failed." << endl;
		return -1;
	}
/*
	// set normal data
	cout << "***********start test***********" << endl;
	testSet("/testn", "testn-data");
	testSet("/testnr/testnr", "testnr-data");
	//
	testCreate("/createn", "createn-data", 'n', false);
	testCreate("/createsn", "createsn-data", 's', false);
	testCreate("/createen", "createen-data", 'e', false);
	testCreate("/createnr/createnr/createnr", "createnr-data", 'n', true);
	testCreate("/createsnr/createsnr/createsnr", "createsnr-data", 's', true);
	testCreate("/createenr/createenr/createenr", "createenr-data", 'e', true);
*/

	zk.watchData("/testw", boost::bind(&dataCallback, _1, _2));
//	zk.watchData("/testw1", boost::bind(&dataCallback, _1, _2));

/*
	zk.watchChildren("/testc", boost::bind(&childrenCallback, _1, _2));
*/
	//
	while(1)
	{
		sleep(1);
	}

	return 0;
}

void testSet(const char *path, const char *data)
{
	// normal node 
	cout << "testSet(\"" << path << "\", \"" << data << ")" << endl;
	ZooKeeper &zk = ZooKeeper::instance(); 
	//
	//
	assert(zk.setData(path, data));
	string value;
	assert(zk.getData(path, value));
	assert(data == value);
}

void testCreate(const char *path, const char *data, char type, bool recursive)
{
	assert('n' == type || 'e' == type || 's' == type);
	//
	cout << "testCreate(\"" << path << "\", \"" << data << ", '" << type << "', " << recursive <<")" << endl;
	//
	ZooKeeper &zk = ZooKeeper::instance(); 
	if('n' == type)
	{
		ZkRet zr = zk.createNode(path, data, recursive);
		assert(zr || zr.nodeExist());
		assert(zk.exists(path));
	}
	else if('e' == type)
	{
		assert(!zk.exists(path));
		assert(zk.createEphemeralNode(path, data, recursive));
		assert(zk.exists(path));
	}
	else if('s' == type)
	{
		string rpath;
		assert(zk.createSequenceNode(path, data, rpath, recursive));
		assert(rpath.find(path) != rpath.npos && rpath != path);
		assert(zk.exists(rpath));
	}

}