#include "MyScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "HelloWorldScene.h"
#include "SecondScene.h"
#include "math.h"

USING_NS_CC;

using namespace cocostudio::timeline;

Scene* SecondScene::createScene()
{
	//��������
	auto scene = Scene::createWithPhysics();
	scene->getPhysicsWorld()->setGravity(Vec2(0, -300));
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_NONE);
	scene->getPhysicsWorld()->setSubsteps(10);

	auto layer = SecondScene::create();

	scene->addChild(layer);

	return scene;
}

bool SecondScene::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}
	//Ԥ����������Ч
	myMusicer = CocosDenshion::SimpleAudioEngine::getInstance();
	myMusicer->preloadBackgroundMusic("bgMusic.mp3");
	myMusicer->preloadBackgroundMusic("button.WAV");
	myMusicer->preloadBackgroundMusic("ready.mp3");
	myMusicer->preloadBackgroundMusic("boom.wav");
	myMusicer->preloadBackgroundMusic("round2.mp3");
	myMusicer->preloadBackgroundMusic("fire1.mp3");
	myMusicer->preloadBackgroundMusic("fire2.mp3");
	myMusicer->preloadBackgroundMusic("dead.mp3");
	myMusicer->preloadBackgroundMusic("star.mp3");
	myMusicer->preloadBackgroundMusic("pass.mp3");

	//��Ƭ��ͼ
	auto map1 = TMXTiledMap::create("map1.tmx");
	addChild(map1, 0, "map1");

	//�������� --����
	auto player = Sprite::create("player.png");
	player->setTag(1);
	player->setPosition(20, 580);
	addChild(player, 0, "player");

	//���ݳ���
	auto playerIn = MoveTo::create(2, Vec2(50, 580));//�����ƶ�
	auto blink = Blink::create(3.0, 5);//3������˸5��
	auto seq0 = Sequence::create(playerIn, blink, NULL);//��˳��ִ��
	player->runAction(seq0);//������

	//����1
	auto star1 = Sprite::create("star.png");
	auto blink1 = Blink::create(60,60);
	star1->runAction(blink1);
	star1->setScale(0.015);
	star1->setPosition(440, 536);
	addChild(star1, 0, "star1");

	//����2
	auto star2 = Sprite::create("star.png");
	star2->setScale(0.02);
	star2->setPosition(860, 600);
	addChild(star2, 0, "star2");

	//�÷�
	auto label0 = Label::create("X 0", "Arial", 30);
	label0->setPosition(910, 600);
	addChild(label0, 0, "label0");

	//�������� --�ϰ���
	auto land = Sprite::create("land.png");
	land->setPosition(300, 530);
	addChild(land, 0, "land");

	//�ϰ���Ѳ��
	auto move = MoveBy::create(3, Vec2(250, 0));
	auto delay1 = DelayTime::create(1);
	auto seq1 = Sequence::create(move, delay1, move->reverse(), delay1, NULL);
	auto repseq = RepeatForever::create(seq1);//����ѭ��
	land->runAction(repseq);

	//��ť
	auto button = Button::create("button.png");
	button->setScale(0.5);
	button->setPosition(ccp(808, 150));
	this->addChild(button,0,"button");

	//������ǩ
	auto label = Label::createWithTTF("Round Two", "FeiGunGun.ttf", 40);
	auto ls = label->getContentSize();
	label->setPosition(-ls.width / 2, 320);
	addChild(label);

	//��ǩ�ƶ�
	auto moveIn = MoveTo::create(2.0, Vec2(480, 320));//�ƶ���ǩ������
	auto moveOut = MoveTo::create(2.0, Vec2(1200, 320));//�Ƴ�
	auto delay = DelayTime::create(1.0);//ͣ��1��
	auto seq = Sequence::create(moveIn, delay, moveOut, NULL);//��˳��ִ��
	label->runAction(seq);//�󶨱�ǩ
	this->scheduleOnce(schedule_selector(SecondScene::RoundTwo), 0.8f);//ready��Ч��ʱ��
	this->scheduleOnce(schedule_selector(SecondScene::Ready), 2.8f);//ready��Ч��ʱ��
	this->scheduleOnce(schedule_selector(SecondScene::Start), 5.0f);//�������ֶ�ʱ��

	scheduleUpdate();

	return true;
}

//��ʱ�� - 1. �ƶ���ʱ�� - �ϰ����⺯��
bool SecondScene::IsCanReach(float x, float y)
{
	auto map1 = (TMXTiledMap*)getChildByName("map1");
	auto mask = map1->layerNamed("mask");//��ȡ�ϰ���ͼ��
	auto TileSize = 16;
	int mapX = x / TileSize;
	int col = map1->getMapSize().height;
	int mapY = col - y / 16;
	if (mask->getTileAt(Vec2(mapX, mapY)))
		return true;
	return false;
}

//��ʱ�� - 1. �ƶ���ʱ�� - ���ؼ�⺯��
bool SecondScene::IsCanPass(float x, float y)
{
	auto map1 = (TMXTiledMap*)getChildByName("map1");
	auto pass = map1->layerNamed("pass");
	auto TileSize = 16;
	int mapX = x / TileSize;
	int col = map1->getMapSize().height;
	int mapY = col - y / 16;
	if (pass->getTileAt(Vec2(mapX, mapY)))
		return true;
	return false;
}

//��ʱ�� - 1. �ƶ���ʱ�� - ���Ǽ�⺯��
bool SecondScene::IsCanGet(float x, float y)
{
	auto map1 = (TMXTiledMap*)getChildByName("map1");
	auto star = map1->layerNamed("star");
	auto TileSize = 16;
	int mapX = x / TileSize;
	int col = map1->getMapSize().height;
	int mapY = col - y / 16;
	if (star->getTileAt(Vec2(mapX, mapY)))
		return true;
	return false;
}

//��ȡ��Сֵ
int SecondScene::min(float x, float y)
{
	if (x > y)
		return y;
	else
		return x;
}

//��ȡ���ֵ
int SecondScene::max(float x, float y)
{
	if (x > y)
		return x;
	else
		return y;
}

//����round2��Ч
void SecondScene::RoundTwo(float){
	myMusicer->playBackgroundMusic("round2.mp3");
}

//����ready��Ч
void SecondScene::Ready(float){
	myMusicer->playBackgroundMusic("ready.mp3");
}

//�̻���Ч
void SecondScene::fire1(float){
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("fire1.mp3");
}

void SecondScene::fire2(float){
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("fire2.mp3");
}

//��Ϸ��ʼ
void SecondScene::Start(float){
	//���ű�������
	myMusicer->playBackgroundMusic("bgMusic.mp3");

	//���ݸ���
	auto player = getChildByName("player");
	playerbody = PhysicsBody::createCircle(20);
	playerbody->applyForce(Vec2(500, 0));
	player->setPhysicsBody(playerbody);

	//��ť
	auto button = Button::create("button.png");
	button->addTouchEventListener(CC_CALLBACK_2(SecondScene::touchMove, this));
	button->setScale(0.5);
	button->setPressedActionEnabled(true);
	button->setPosition(ccp(808, 150));
	this->addChild(button,0,"button1");

	//��ť������
	auto listener = EventListenerTouchOneByOne::create();
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
}

//ִ���ƶ�
void SecondScene::toch_Move(float dt)
{
	if (getChildByTag(1))
		playerbody->applyImpulse(Vec2(0, 1000));
}

//������ť�¼�
void SecondScene::touchMove(cocos2d::Ref* pSender, Widget::TouchEventType event)
{
	if (event == Widget::TouchEventType::BEGAN)
	{
		this->schedule(schedule_selector(SecondScene::toch_Move), 0.0f);
	}

	if (event == Widget::TouchEventType::ENDED)
	{
		this->unschedule(schedule_selector(SecondScene::toch_Move));
	}
}

//��ʱ�� - ��ײ���
void SecondScene::update(float dt)
{
	auto player = getChildByName("player");
	auto land = getChildByName("land");
	auto star1 = getChildByName("star1");
	auto label0 = getChildByName("label0");

	//Բ�;�����ײ�㷨
	int x = (player->getPositionX()) - (land->getPositionX());
	int y = (player->getPositionY()) - (land->getPositionY());
	int minX = min(x, 24);
	int maxX = max(minX, -24);
	int minY = min(y, 16);
	int maxY = max(minY, -16);

	//�ϰ�����ײ���
	if (((maxX - x)*(maxX - x) + (maxY - y) *(maxY - y) <= 400) || IsCanReach(player->getPositionX() + 20, player->getPositionY() + 20) || IsCanReach(player->getPositionX() - 20, player->getPositionY() - 20))
	{	
		//��Ϸ����
		//unschedule("move");
		land->stopAllActions();
		//listener->setEnabled(false);

		//������� - ���� - 3. ���Ǳ�ը����
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("boom.wav");

		//������� - 3. ��ը����
		auto SFC = SpriteFrameCache::getInstance();
		SFC->addSpriteFramesWithFile("boom.plist", "boom.png");
		auto animation = Animation::create();
		char name[20];
		for (int i = 0; i <= 15; i++)
		{
			sprintf(name, "boom%d.png", i);
			animation->addSpriteFrame(SFC->getSpriteFrameByName(name));
		}
		animation->setDelayPerUnit(0.1);
		animation->setLoops(1);
		auto animate = Animate::create(animation);
		auto explode = Sprite::createWithSpriteFrameName("boom0.png");
		explode->setPosition(player->getPosition());
		auto removeExplode = CallFunc::create(CC_CALLBACK_0(SecondScene::ExplodeRemove, this));
		auto seq = Sequence::create(animate, removeExplode, NULL);
		explode->runAction(seq);
		addChild(explode, 0, "explode");

		//���پ���
		unscheduleUpdate();
		removeChild(player);

		auto dict3 = Dictionary::createWithContentsOfFile("chn.xml");
		auto chn3 = ((String*)dict3->objectForKey("over"))->getCString();
		auto label3 = Label::createWithTTF(chn3, "FeiGunGun.ttf", 40);
		label3->setPosition(480, 400);
		addChild(label3);

		//�����˵�
		ShowMenu();
		return;
	}

	//���ؼ��
	else if (IsCanPass(player->getPositionX() + 7, player->getPositionY() + 7) || IsCanPass(player->getPositionX() - 7, player->getPositionY() - 7))
	{
		if (!label0){
			//���پ���
			unscheduleUpdate();
			removeChild(player);

			//�̻�
			this->scheduleOnce(schedule_selector(SecondScene::fire1), 0.0f);
			this->scheduleOnce(schedule_selector(SecondScene::fire2), 1.0f);

			//����ϵͳ  -  �̻�
			/*auto particle = ParticleSystemQuad::create("fire.plist");
			particle->setPosition(480,320);
			addChild(particle);*/

			//������� - 3. �̻�����
			auto SFC1 = SpriteFrameCache::getInstance();
			SFC1->addSpriteFramesWithFile("fire.plist", "fire.png");
			auto animation1 = Animation::create();
			char name1[20];
			for (int i = 0; i <= 15; i++)
			{
				sprintf(name1, "fire%d.png", i);
				animation1->addSpriteFrame(SFC1->getSpriteFrameByName(name1));
			}
			animation1->setDelayPerUnit(0.1);
			animation1->setLoops(1);
			auto animate1 = Animate::create(animation1);
			auto explode1 = Sprite::createWithSpriteFrameName("fire0.png");
			explode1->setPosition(480,500);
			auto removeExplode1 = CallFunc::create(CC_CALLBACK_0(SecondScene::ExplodeRemove, this));
			auto fire = Sprite::create("fire0.png");
			fire->setPosition(480,-10);
			addChild(fire,0,"fire");
			auto firemove = MoveTo::create(1,Vec2(480,500));
			auto seq1 = Sequence::create(firemove, animate1, removeExplode1, NULL);
			fire->runAction(seq1);
			addChild(fire, 0, "explode1");
			this->scheduleOnce(schedule_selector(SecondScene::remove), 1.0f);

			//��ʾͨ��
			auto dict2 = Dictionary::createWithContentsOfFile("chn.xml");
			auto chn2 = ((String*)dict2->objectForKey("win"))->getCString();
			auto label2 = Label::createWithTTF(chn2, "FeiGunGun.ttf", 40);
			label2->setPosition(480, 430);
			addChild(label2);

			//�����˵�
			ShowMenu1();
		}
		else
		{
			//���پ���
			unscheduleUpdate();
			removeChild(player);

			//������������
			myMusicer->playBackgroundMusic("dead.mp3");

			//��ʾʧ��
			auto dict2 = Dictionary::createWithContentsOfFile("chn.xml");
			auto chn2 = ((String*)dict2->objectForKey("lose"))->getCString();
			auto label2 = Label::createWithTTF(chn2, "FeiGunGun.ttf", 40);
			label2->setPosition(480, 450);
			addChild(label2);
			auto dict3 = Dictionary::createWithContentsOfFile("chn.xml");
			auto chn3 = ((String*)dict3->objectForKey("over"))->getCString();
			auto label3 = Label::createWithTTF(chn3, "FeiGunGun.ttf", 40);
			label3->setPosition(480, 400);
			addChild(label3);

			//�����˵�
			ShowMenu();
		}
		return;
	}
	else if (IsCanGet(player->getPositionX(), player->getPositionY()))
	{
		//���ŵ÷�����
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("star.mp3");

		//�������Ǻ͵÷�0
		removeChild(star1);
		removeChild(label0);

		//�÷�1
		auto label1 = Label::create("X 1", "Arial", 30);
		label1->setPosition(910, 600);
		addChild(label1, 0, "label1");
		
		//unscheduleUpdate();
		return;
	}
}


//���ٱ�ը
void SecondScene::ExplodeRemove()
{
	removeChildByName("explode");
	removeChildByName("explode1");
}

//�����̻�
void SecondScene::remove(float)
{
	removeChildByName("fire");
}


//ʧ�ܲ˵�
void SecondScene::ShowMenu()
{
	//������������
	myMusicer->playBackgroundMusic("dead.mp3");

	if (!getChildByName("menu"))
	{
		auto resumeLabel = Label::create("Resume", "Arial", 30);
		auto resumeMI = MenuItemLabel::create(resumeLabel, this, menu_selector(SecondScene::progress1));
		resumeMI->setPosition(-100, 0);
		auto exitLabel = Label::create("Exit", "Arial", 30);
		auto exitMI = MenuItemLabel::create(exitLabel, this, menu_selector(SecondScene::exit));
		exitMI->setPosition(100, 0);
		auto menu = Menu::create(resumeMI, exitMI, NULL);
		addChild(menu, 0, "menu");
	}
	else
		getChildByName("menu")->setVisible(true);
}

//ͨ���˵�
void SecondScene::ShowMenu1()
{
	//���Ź�������
	myMusicer->playBackgroundMusic("pass.mp3");

	//��ʾ�˵�
	if (!getChildByName("menu1"))
	{
		auto homeLabel = Label::create("Home", "Arial", 30);
		auto homeMI = MenuItemLabel::create(homeLabel, this, menu_selector(SecondScene::progress));
		homeMI->setPosition(-150, 0);
		auto resumeLabel = Label::create("Resume", "Arial", 30);
		auto resumeMI = MenuItemLabel::create(resumeLabel, this, menu_selector(SecondScene::progress1));
		resumeMI->setPosition(0, 0);
		auto exitLabel = Label::create("Exit", "Arial", 30);
		auto exitMI = MenuItemLabel::create(exitLabel, this, menu_selector(SecondScene::exit));
		exitMI->setPosition(150, 0);
		auto menu1 = Menu::create(homeMI, resumeMI,exitMI, NULL);
		addChild(menu1, 0, "menu1");
	}
	else
		getChildByName("menu1")->setVisible(true);
}

//���ؽ�����
void SecondScene::progress(Ref* pSender)
{
	//��ť�����Ч
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("button.WAV");

	//�ص�����
	if (myMusicer->isBackgroundMusicPlaying()){
		myMusicer->stopBackgroundMusic();
	}

	//����
	auto menu1 = getChildByName("menu1");
	auto label = getChildByName("label");
	auto label2 = getChildByName("label2");
	auto map1 = getChildByName("map1");
	auto button = getChildByName("button");
	auto button1 = getChildByName("button1");
	auto star1 = getChildByName("star1");
	auto star2 = getChildByName("star2");
	auto land = getChildByName("land");
	auto label1 = getChildByName("label1");
	removeChild(menu1);
	removeChild(label);
	removeChild(label2);
	removeChild(map1);
	removeChild(button);
	removeChild(button1);
	removeChild(star1);
	removeChild(star2);
	removeChild(land);
	removeChild(label1);

	auto ui = TMXTiledMap::create("ui.tmx");
	addChild(ui, 0, "ui");

	//��ʾ��ҳ
	auto dict3 = Dictionary::createWithContentsOfFile("chn.xml");
	auto chn3 = ((String*)dict3->objectForKey("home"))->getCString();
	auto label3 = Label::createWithTTF(chn3, "FeiGunGun.ttf", 80);
	label3->setPosition(480, 450);
	addChild(label3);

	//���ر�ǩ
	auto loadLabel = Label::create("Loading...", "Arial", 30);
	loadLabel->setPosition(480, 300);
	addChild(loadLabel);

	//���ȿ�
	auto kuang = Sprite::create("loadKuang.png");
	kuang->setPosition(Vec2(480, 200));
	addChild(kuang);

	//������
	auto progress2 = ProgressFromTo::create(5, 0, 100);     //5���0%��100%
	auto progTime2 = ProgressTimer::create(Sprite::create("loadBar.png"));
	progTime2->setPosition(Vec2(484, 200));
	this->addChild(progTime2);

	//����
	progTime2->setType(ProgressTimer::Type::BAR);
	progTime2->setBarChangeRate(Vec2(1, 0));      //���ý�����Ϊ�����ƶ��������ƶ���Vec2��0��1��
	progTime2->setMidpoint(Vec2(0, 0));        //���ý��������������ƶ������������ƶ���Vec2��1��0��
	progTime2->runAction(progress2);

	this->scheduleOnce(schedule_selector(SecondScene::home), 5.0f);
}

//ʧ�ܽ�����
void SecondScene::progress1(Ref* pSender)
{
	//��ť�����Ч
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("button.WAV");

	//�ص�����
	if (myMusicer->isBackgroundMusicPlaying()){
		myMusicer->stopBackgroundMusic();
	}

	//����
	auto menu1 = getChildByName("menu1");
	auto label = getChildByName("label");
	auto label2 = getChildByName("label2");
	auto map1 = getChildByName("map1");
	auto button = getChildByName("button");
	auto button1 = getChildByName("button1");
	auto star1 = getChildByName("star1");
	auto star2 = getChildByName("star2");
	auto land = getChildByName("land");
	auto label0 = getChildByName("label0");
	removeChild(menu1);
	removeChild(label);
	removeChild(label2);
	removeChild(map1);
	removeChild(button);
	removeChild(button1);
	removeChild(star1);
	removeChild(star2);
	removeChild(land);
	removeChild(label0);

	auto ui = TMXTiledMap::create("ui.tmx");
	addChild(ui, 0, "ui");

	//��ʾ�ڶ���
	auto dict3 = Dictionary::createWithContentsOfFile("chn.xml");
	auto chn3 = ((String*)dict3->objectForKey("second"))->getCString();
	auto label3 = Label::createWithTTF(chn3, "FeiGunGun.ttf", 80);
	label3->setPosition(480, 450);
	addChild(label3);

	//���ر�ǩ
	auto loadLabel = Label::create("Loading...", "Arial", 30);
	loadLabel->setPosition(480, 300);
	addChild(loadLabel);

	//���ȿ�
	auto kuang = Sprite::create("loadKuang.png");
	kuang->setPosition(Vec2(480, 200));
	addChild(kuang);

	//������
	auto progress2 = ProgressFromTo::create(5, 0, 100);     //5���0%��100%
	auto progTime2 = ProgressTimer::create(Sprite::create("loadBar.png"));
	progTime2->setPosition(Vec2(484, 200));
	this->addChild(progTime2);

	//����
	progTime2->setType(ProgressTimer::Type::BAR);
	progTime2->setBarChangeRate(Vec2(1, 0));      //���ý�����Ϊ�����ƶ��������ƶ���Vec2��0��1��
	progTime2->setMidpoint(Vec2(0, 0));        //���ý��������������ƶ������������ƶ���Vec2��1��0��
	progTime2->runAction(progress2);

	//��ʾ
	auto dict4 = Dictionary::createWithContentsOfFile("chn.xml");
	auto chn4 = ((String*)dict4->objectForKey("tips"))->getCString();
	auto label4 = Label::createWithTTF(chn4, "FeiGunGun.ttf", 25);
	label4->setPosition(480, 170);
	addChild(label4);

	this->scheduleOnce(schedule_selector(SecondScene::resume), 5.0f);
}


//�˵� - 2. resume����
void SecondScene::resume(float)
{
	Director::getInstance()->replaceScene(TransitionProgressOutIn::create(1, SecondScene::createScene()));
}

//�˵� - 2. home����
void SecondScene::home(float)
{
	Director::getInstance()->replaceScene(TransitionProgressOutIn::create(1, HelloWorld::createScene()));
}

//�˵� - 2. exit����
void SecondScene::exit(Ref* pSender)
{
	Director::getInstance()->end();
}