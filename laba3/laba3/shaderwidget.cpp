#include "shaderwidget.h"

void ShaderWidget::initializeGL(){
    glClearColor(1.,1.,1.,1.);

    QOpenGLShader vShader(QOpenGLShader::Vertex);
    vShader.compileSourceFile("D:/c++/NNSU/2class/QT/laba3/laba3/simple.vert");

    QOpenGLShader fShader(QOpenGLShader::Fragment);
    fShader.compileSourceFile("D:/c++/NNSU/2class/QT/laba3/laba3/simple.frag");

    prog.addShader(&vShader);
    prog.addShader(&fShader);
    if (!prog.link()){
        qWarning("Error link programm");
        return;
    }
    pos = prog.attributeLocation("vertex");

    qDebug() << QString("Log programm");
    qDebug() << prog.log();

    if (!prog.bind()){
        qWarning("Error bind programm");
    }
    /*
    prog.setUniformValue("camera.position", QVector3D(camera.pos.x(), camera.pos.y(), camera.pos.z()));
    prog.setUniformValue("camera.view", QVector3D(0, 0, 1));
    prog.setUniformValue("camera.up", QVector3D(0, 1, 0));
    prog.setUniformValue("camera.side", QVector3D(1, 0, 0));
    prog.setUniformValue("scale", QVector2D(width(), height()));
    prog.setUniformValue("light_pos", QVector3D(lightX, lightY, lightZ));
    prog.setUniformValue("scale", QVector2D(width(), height()));
    prog.setUniformValue("light_pos", QVector3D(lightX, lightY, lightZ));*/


    std::vector<Sphere> all_spheres;
    all_spheres.push_back(Sphere{ QVector3D(-10, 0, 0), 2, QVector3D(1.0, 1.0, 0), 0});

    all_spheres.push_back(Sphere{ QVector3D(10, 0, 0), 2, QVector3D(0.52, 0, 0.52), 0});
    all_spheres.push_back(Sphere{ QVector3D(0, 10, 0), 2, QVector3D(1, 0, 0), 0});
    all_spheres.push_back(Sphere{ QVector3D(0, -10, 0), 2, QVector3D(0, 0, 1), 0});
    all_spheres.push_back(Sphere{ QVector3D(0, 0, 10), 2, QVector3D(0, 1, 0), 0});
    all_spheres.push_back(Sphere{ QVector3D(0, 0, -10), 2, QVector3D(0, 0.5, 1), 0});
    all_spheres.push_back(Sphere{ QVector3D(0, 0, 0), 9, QVector3D(1, 1, 1), 0});

    all_spheres.push_back(Sphere{ QVector3D(sqrt(2) / 2 * 9, 0, -sqrt(2) / 2 * 9), 2, QVector3D(0, 0.25, 0), 0});
    all_spheres.push_back(Sphere{ QVector3D(-sqrt(2) / 2 * 9, 0, -sqrt(2) / 2 * 9), 2, QVector3D(0, 0.5, 0), 0});
    all_spheres.push_back(Sphere{ QVector3D(-sqrt(2) / 2 * 9, 0, sqrt(2) / 2 * 9), 2, QVector3D(0, 0.75, 0), 0});
    all_spheres.push_back(Sphere{ QVector3D(sqrt(2) / 2 * 9, 0, sqrt(2) / 2 * 9), 2, QVector3D(0, 1.0, 0), 0});

    all_spheres.push_back(Sphere{ QVector3D(sqrt(2) / 2 * 9, -sqrt(2) / 2 * 9, 0), 2, QVector3D(0.25, 0, 0), 0});
    all_spheres.push_back(Sphere{ QVector3D(-sqrt(2) / 2 * 9, -sqrt(2) / 2 * 9, 0), 2, QVector3D(0.5, 0, 0), 0});
    all_spheres.push_back(Sphere{ QVector3D(-sqrt(2) / 2 * 9, sqrt(2) / 2 * 9, 0), 2, QVector3D(0.75, 0, 0), 0});
    all_spheres.push_back(Sphere{ QVector3D(sqrt(2) / 2 * 9, sqrt(2) / 2 * 9, 0), 2, QVector3D(1.0, 0, 0), 0});

    all_spheres.push_back(Sphere{ QVector3D(0, sqrt(2) / 2 * 9, -sqrt(2) / 2 * 9), 2, QVector3D(0, 0, 0.25), 0});
    all_spheres.push_back(Sphere{ QVector3D(0, -sqrt(2) / 2 * 9, -sqrt(2) / 2 * 9), 2, QVector3D(0, 0, 0.5), 0});
    all_spheres.push_back(Sphere{ QVector3D(0, -sqrt(2) / 2 * 9, sqrt(2) / 2 * 9), 2, QVector3D(0, 0, 0.75), 0});
    all_spheres.push_back(Sphere{ QVector3D(0, sqrt(2) / 2 * 9, sqrt(2) / 2 * 9), 2, QVector3D(0, 0, 1.0), 0});

    prog.setUniformValue("vector_size", (int)all_spheres.size());

    functions = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_3_Core>();
    functions->glGenBuffers(1, &ssbo);
    functions->glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    functions->glBufferData(GL_SHADER_STORAGE_BUFFER, all_spheres.size() * sizeof(Sphere), all_spheres.data(), GL_DYNAMIC_COPY);
    functions->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
    prog.release();
}

void ShaderWidget::resizeGL(int nwidth, int nheight){
    glViewport(0,0,nwidth, nheight);
}

void ShaderWidget::paintGL(){
    glClear(GL_COLOR_BUFFER_BIT);
    if (!prog.bind()){
        return;
    }
    prog.enableAttributeArray(pos);
    prog.setAttributeArray(pos, data, 3);

    glDrawArrays(GL_QUADS, 0, 4);

    prog.disableAttributeArray(pos);
    prog.setUniformValue("camera.position", QVector3D(camera.pos.x(), camera.pos.y(), camera.pos.z()));
    prog.setUniformValue("camera.view", QVector3D(camera.view.x(), camera.view.y(), camera.view.z()));
    prog.setUniformValue("camera.up", QVector3D(camera.up.x(), camera.up.y(), camera.up.z()));
    prog.setUniformValue("camera.side", QVector3D(camera.side.x(), camera.side.y(), camera.side.z()));
    prog.setUniformValue("scale", QVector2D(width(), height()));
    prog.setUniformValue("light_pos", QVector3D(lightX, lightY, lightZ));
    prog.release();
    qDebug() << "Paint GL " << camera.pos.x() << ' ' << camera.pos.y() << ' ' << camera.pos.z();
    qDebug() << "Paint GL " << camera.view.x() << ' ' << camera.view.y() << ' ' << camera.view.z();
    qDebug() << "Paint GL " << camera.up.x() << ' ' << camera.up.y() << ' ' << camera.up.z();
    qDebug() << "Paint GL " << camera.side.x() << ' ' << camera.side.y() << ' ' << camera.side.z();
}

ShaderWidget::ShaderWidget(QWidget *parent, int x, int y, int z): QOpenGLWidget(parent), pos(0), lightX(x), lightY(y), lightZ(z){
    data = new GLfloat[12];
    data[0] = -1.;  data[1] = -1.;  data[2] = 0.;
    data[3] = 1.;   data[4] = -1.;  data[5] = 0;
    data[6] = 1.;   data[7] = 1.;   data[8] = 0;
    data[9] = -1.;  data[10] = 1.;  data[11] = 0;
    ang = PI / 2 * 3;
    camera.R = 50;
    camera.pos.setX(cos(ang) * camera.R);
    camera.pos.setY(0);
    camera.pos.setZ(sin(ang) * camera.R);
    camera.view.setX(cos(ang - PI));
    camera.view.setY(0);
    camera.view.setZ(sin(ang - PI));
    camera.up.setX(0);
    camera.up.setY(1);
    camera.up.setZ(0);
    camera.side.setX(cos(ang - 3 * PI / 2));
    camera.side.setY(0);
    camera.side.setZ(sin(ang - 3 * PI / 2));
}

ShaderWidget::~ShaderWidget(){
    delete[] data;
}

void ShaderWidget::keyPressEvent(QKeyEvent *event){
    update();
    qDebug() << event->nativeVirtualKey();
    if (event->nativeVirtualKey() == 38){
        ang += 0.01;
        camera.pos.setX(cos(ang) * camera.R);
        camera.pos.setZ(sin(ang) * camera.R);
        camera.view.setX(cos(ang - PI));
        camera.view.setZ(sin(ang - PI));
        camera.side.setX(cos(ang - 3 * PI / 2));
        camera.side.setZ(sin(ang - 3 * PI / 2));
    }
    if (event->nativeVirtualKey() == 40){
        ang -= 0.01;
        camera.pos.setX(cos(ang) * camera.R);
        camera.pos.setZ(sin(ang) * camera.R);
        camera.view.setX(cos(ang - PI));
        camera.view.setZ(sin(ang - PI));
        camera.side.setX(cos(ang - 3 * PI / 2));
        camera.side.setZ(sin(ang - 3 * PI / 2));
    }
    if (event->nativeVirtualKey() == Qt::Key_Q){
        lightX += 3;
        //qDebug() << "LOL";
    }
    if (event->nativeVirtualKey() == Qt::Key_W){
        lightY += 3;
        //qDebug() << "KEK";
    }
    if (event->nativeVirtualKey() == Qt::Key_E){
        lightZ += 3;
        //qDebug() << "QWE";
    }
    if (event->nativeVirtualKey() == Qt::Key_A){
        lightX -= 3;
        //qDebug() << "LOL";
    }
    if (event->nativeVirtualKey() == Qt::Key_S){
        lightY -= 3;
        //qDebug() << "KEK";
    }
    if (event->nativeVirtualKey() == Qt::Key_D){
        lightZ -= 3;
        //qDebug() << "QWE";
    }
    update();
}

