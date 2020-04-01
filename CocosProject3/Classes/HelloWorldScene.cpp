#include "HelloWorldScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "MyScene.h"
#include "SecondScene.h"

USING_NS_CC;

using namespace cocostudio::timeline;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
	auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
	
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}
	//加载音乐、音效
	myMusicer = CocosDenshion::SimpleAudioEngine::getInstance();
	myMusicer->preloadBackgroundMusic("bgMusic.mp3");
	myMusicer->preloadBackgroundMusic("uiMusic.mp3");
	myMusicer->preloadBackgroundMusic("button.WAV");
	myMusicer->preloadBackgroundMusic("ready.mp3");
	myMusicer->playBackgroundMusic("uiMusic.mp3");

	//瓦片地图
	auto ui = TMXTiledMap::create("ui.tmx");
	addChild(ui, 0, "ui");

	//显示中文
	auto dict = Dictionary::createWithContentsOfFile("chn.xml");
	auto chn = ((String*)dict->objectForKey("str"))->getCString();
	auto label = Label::createWithTTF(chn, "FeiGunGun.ttf", 80);
	label->setPosition(480, 450);
	addChild(label,0,"label");

	//提示
	auto dict3 = Dictionary::createWithContentsOfFile("chn.xml");
	auto chn3 = ((String*)dict3->objectForKey("tips"))->getCString();
	auto label3 = Label::createWithTTF(chn3, "FeiGunGun.ttf", 25);
	label3->setPosition(480, 170);
	addChild(label3);

	//菜单
	const char* start = ((String*)dict->objectForKey("StartStr"))->getCString();
	auto label1 = Label::create(start, "黑体", 30);

	const char* end = ((String*)dict->objectForKey("ExitStr"))->getCString();
	auto label2 = Label::create(end, "黑体", 30);

	auto MIlabel1 = MenuItemLabel::create(label1, this, menu_selector(HelloWorld::progress));
	MIlabel1->setPosition(0, 3);
	auto MIlabel2 = MenuItemLabel::create(label2, this, menu_selector(HelloWorld::endit));
	MIlabel2->setPosition(0, -77);
	auto MIimage1 = MenuItemImage::create("button2.png", "button1.png", this, menu_selector(HelloWorld::progress));
	MIimage1->setPosition(0, 0);
	auto MIimage2 = MenuItemImage::create("button2.png", "button1.png", this, menu_selector(HelloWorld::endit));
	MIimage2->setPosition(0, -80);

	auto mymenu1 = Menu::create(MIimage1, MIlabel1, NULL);
	auto mymenu2 = Menu::create(MIimage2, MIlabel2, NULL);

	this->addChild(mymenu1,0,"mymenu1");
	this->addChild(mymenu2,0,"mymenu2");

	return true;
}

//进度条
void HelloWorld::progress(Ref* pSender)
{
	//按钮点击音效
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("button.WAV");
	
	//关掉音乐
	if (myMusicer->isBackgroundMusicPlaying()){
		myMusicer->stopBackgroundMusic();
	}

	//完善
	auto mymenu1 = getChildByName("mymenu1");
	auto mymenu2 = getChildByName("mymenu2");
	auto label = getChildByName("label");
	removeChild(mymenu1);
	removeChild(mymenu2);
	removeChild(label);

	//显示第一关
	auto dict1 = Dictionary::createWithContentsOfFile("chn.xml");
	auto chn1 = ((String*)dict1->objectForKey("first"))->getCString();
	auto label1 = Label::createWithTTF(chn1, "FeiGunGun.ttf", 80);
	label1->setPosition(480, 450);
	addChild(label1);

	//加载标签
	auto loadLabel = Label::create("Loading...", "Arial", 30);
	loadLabel->setPosition(480,300);
	addChild(loadLabel);

	//进度框
	auto kuang = Sprite::create("loadKuang.png");
	kuang->setPosition(Vec2(480, 200));
	addChild(kuang);

	//进度条
	auto progress2 = ProgressFromTo::create(5, 0, 100);     //5秒从0%到100%
	auto progTime2 = ProgressTimer::create(Sprite::create("loadBar.png"));
	progTime2->setPosition(Vec2(484, 200));
	this->addChild(progTime2);

	//条形
	progTime2->setType(ProgressTimer::Type::BAR);
	progTime2->setBarChangeRate(Vec2(1, 0));      //设置进度条为横向移动，纵向移动：Vec2（0，1）
	progTime2->setMidpoint(Vec2(0, 0));        //设置进度条从左往右移动，从右往左移动：Vec2（1，0）
	progTime2->runAction(progress2);

	this->scheduleOnce(schedule_selector(HelloWorld::startit), 5.0f);
}

//菜单 - 开始功能,场景切换
void HelloWorld::startit(float)
{
	Director::getInstance()->replaceScene(TransitionProgressOutIn::create(1, MyScene::createScene()));
}

//菜单 - 退出功能
void HelloWorld::endit(Ref* pSender)
{
	Director::getInstance()->end();
}
