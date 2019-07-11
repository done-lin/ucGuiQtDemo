#include "dialogmain.h"
#include "ui_dialogmain.h"

#include <QThread>
#include "GUI.h"
#include "LCDConf.h"

#include <QPushButton>
#include <QWidget>
#include <QTimer>
#include <QImage>
#include <QDebug>
#include <QMouseEvent>

unsigned int LCD_Buffer[LCD_YSIZE][LCD_XSIZE];





#ifdef __cplusplus
extern "C" {
#endif

	void LCDSIM_Init(void)
	{
	}

	void LCDSIM_SetPixelIndex(int x, int y, int Index, int LayerIndex)
	{
		//return;
#if (LCD_BITSPERPIXEL == 8)
		unsigned char R = 1.164 * (Index - 16);
		LCD_Buffer[y][x] = R << 16 | R << 8 | R;
#elif (LCD_BITSPERPIXEL == 16) && (LCD_FIXEDPALETTE == 565)
		LCD_Buffer[y][x] = ((Index & 0xF800) << 3 >> 11 << 16) | ((Index & 0x07E0) << 2 >> 5 << 8) | (Index & 0x1F) << 3;
#elif (LCD_BITSPERPIXEL == 1)
		if (Index == 0)
		{
			LCD_Buffer[y][x] = 0x00000000;
		}
		else
		{
			LCD_Buffer[y][x] = 0x00FFFFFF;
		}
#else
		LCD_Buffer[y][x] = Index;
#endif
	}


	void LCDSIM_FillRect(int x0, int y0, int x1, int y1, int Index, int LayerIndex)
	{

	}


	int LCDSIM_GetPixelIndex(int x, int y, int LayerIndex)
	{
#if (LCD_BITSPERPIXEL == 16) && (LCD_FIXEDPALETTE == 565)
        int pixelValue = LCD_Buffer[y][x];
        return ((pixelValue & 0xF80000) >> 8) | ((pixelValue & 0x00FC00) >> 4) | ((pixelValue & 0x0000F8) >> 3);
#else
		return LCD_Buffer[y][x];
#endif
	}

	void LCDSIM_SetLUTEntry(U8 Pos, LCD_COLOR color, int LayerIndex)
	{

	}

#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
extern "C" {
	extern void MainTask(void);
}
#endif


class GUIThread : public QThread
{
public:
    void run()
    {
        MainTask();  //GUI Demo
    }
};



void DialogMain::mousePressEvent(QMouseEvent *e)
{
    GUI_PID_STATE Touch_Status;

    Touch_Status.Pressed = 1;
    Touch_Status.x = e->x();
    Touch_Status.y = e->y();
    GUI_TOUCH_StoreStateEx(&Touch_Status);
}

void DialogMain::mouseReleaseEvent(QMouseEvent *e)
{
    GUI_PID_STATE Touch_Status;

    Touch_Status.Pressed = 0;
    Touch_Status.x = e->x();
    Touch_Status.y = e->y();
    GUI_TOUCH_StoreStateEx(&Touch_Status);
}

void DialogMain::timerAction()
{
	//qDebug() << "timer action";
	//QByteArray imageByteArray = QByteArray((const char*)LCD_Buffer, LCD_XSIZE*LCD_YSIZE*3);
	//uchar*  transData = (unsigned char*)imageByteArray.data();

	QImage image(LCD_XSIZE, LCD_YSIZE, QImage::Format_RGB32);
	for (int i = 0; i < LCD_XSIZE; i++)
	{
		for (int j = 0; j < LCD_YSIZE; j++)
		{
			unsigned int value = (uint)(LCD_Buffer[j][i]) ;
			image.setPixel(i, j, value);
		}
	}

	ui->label->setPixmap(QPixmap::fromImage(image));

	this->update();
}


DialogMain::DialogMain(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogMain)
{
    ui->setupUi(this);

	GUIThread* guithread = new GUIThread();
	guithread->start();

	QImage myImage;
	myImage.load("C:\\Android\\Android\\sdk\\docs\\images\\home\\androidn-hero-n5.png");

	ui->label->setPixmap(QPixmap::fromImage(myImage));

	
	//image->
	timer = new QTimer(this);
	timer->setInterval(30);
	connect(timer, &QTimer::timeout, this, &DialogMain::timerAction);

	timer->start();

    connect(ui->pushButtonUp, &QPushButton::clicked, this, &DialogMain::slotDirectionKeyPress);
    connect(ui->pushButtonDown, &QPushButton::clicked, this, &DialogMain::slotDirectionKeyPress);
    connect(ui->pushButtonLeft, &QPushButton::clicked, this, &DialogMain::slotDirectionKeyPress);
    connect(ui->pushButtonRight, &QPushButton::clicked, this, &DialogMain::slotDirectionKeyPress);
}

DialogMain::~DialogMain()
{
    delete ui;
}

void DialogMain::slotDirectionKeyPress(bool checked)
{
    if(sender() == ui->pushButtonUp)
    {
        GUI_StoreKeyMsg(GUI_KEY_UP, 1);
    }
    else if(sender() == ui->pushButtonDown)
    {
        GUI_StoreKeyMsg(GUI_KEY_DOWN, 1);
    }
    else if(sender() == ui->pushButtonLeft)
    {
        GUI_StoreKeyMsg(GUI_KEY_LEFT, 1);
    }
    else if(sender() == ui->pushButtonRight)
    {
        GUI_StoreKeyMsg(GUI_KEY_RIGHT, 1);
    }
}
