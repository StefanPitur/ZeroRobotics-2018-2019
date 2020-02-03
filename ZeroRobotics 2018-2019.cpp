int parte;
float posicionxd[12];
float p9[3];
float p1[3];
float p6[3];
float p7[3];
float myPos[3];
float myVel[3];
float vel5[3];
float vel6[3];
float p8[3];
bool check;
bool colision;
float quat[4];
float angulo[1000];
float rota[3];
float sphvel[3];
float debris[41][4];
float myState[12];
float vel2[3];
float vel3[3];
float vel4[3];
int loop_cnt,scnt;
int t;
float vel1[3];
int ansgreen,anspurple,ansorange;
float pi;
float R;
float E;
float i,k;
float DIVIDE,inf,VAL,arrive;
int alegere;

struct Corners{
    float x,z;
} corners[5];
struct Travel{
    float coords[3];
    bool reach;
} A,B,C,D,F;
struct spts{
    bool fit;
    float coords[3];
} spots[23];
float best[10001][4];
int debrishelp[5];

float min(float x, float y){
    return x<y?x:y;
}

float max(float x, float y){
    return x>y?x:y;
}

float distance(float x, float z, float dx, float dz, float r){
    float d=(float)((dx-x)*(dx-x)+(dz-z)*(dz-z));
    d=sqrtf(d);
    if(d>VAL+r)
        return d;
    return 0;
}

void init()
{
    parte=0;
    api.getOtherSphState(angulo);
    api.getMyZRState(myState);
    quat[0] = angulo[6];
    quat[1] = angulo[7];
    quat[2] = angulo[8];
    quat[3] = angulo[9];

    pi=3.141592653f;
    R=0.123264f;
    E=0.01f;
    DIVIDE=100.0f;
    inf=9999.0f;
    VAL=R+E;
    loop_cnt=api.getTime();
    scnt=0;
    arrive=0.04f;
    vel1[0] = (myState[3]+myState[3]+myState[3]+myState[3]);
    vel1[1] = -0.045;
    vel1[2] = (myState[5]+myState[5]+myState[5]+myState[5]);
    vel2[0] = 0;
    vel2[1] = -0.010;
    vel2[2] = 0;
    vel3[0] = (myState[3]+myState[3]+myState[3]+myState[3]);
    vel3[1] = 0.99;
    vel3[2] = (myState[5]+myState[5]+myState[5]+myState[5]);
    vel4[0] = (myState[3]+myState[3]+myState[3]+myState[3]);
    vel4[1] = 0.99;
    vel4[2] = (myState[5]+myState[5]+myState[5]+myState[5]);
    vel5[0] = 0;
    vel5[1] = (myState[4]*-1);
    vel5[2] = 0;
    p6[0] = angulo[0];
    p6[1] = 0.5;
    p6[2] = angulo[2];
    p7[0] = 0.2;
    p7[1] = 0.7;
    p7[2] = -0.1;
}

void solve(int h1, int h2, int hr, int ad1, int ad2, int adr, int add){
    float d,minime,maxim;
    int aux,puncte,pozmax,d1,d2,d3,d4;
    for(int h=h1; h<=h2; h+=hr)
        for(int ad=ad1; ad<=ad2; ad+=adr){
            d1=h+ad-1, d2=h+ad, d3=h+ad+(add-1), d4=h+ad+add;
            debrishelp[1]=d1, debrishelp[2]=d2, debrishelp[3]=d3, debrishelp[4]=d4;
            corners[1].x=corners[3].x=max(debris[d1][0],debris[d3][0]);
            corners[2].x=corners[4].x=min(debris[d2][0],debris[d4][0]);
            corners[1].z=corners[2].z=max(debris[d1][2],debris[d2][2]);
            corners[3].z=corners[4].z=min(debris[d3][2],debris[d4][2]);
            i=(float)(corners[2].x-corners[1].x)/DIVIDE;
            k=(float)(corners[3].z-corners[1].z)/DIVIDE;
            puncte=0;
            for(float z=corners[1].z; z<=corners[3].z; z+=k)
                for(float x=corners[1].x; x<=corners[2].x; x+=i){
                    aux=1;
                    minime=inf;
                    while(aux<=4){
                        d=distance(x,z,debris[debrishelp[aux]][0],debris[debrishelp[aux]][2],debris[debrishelp[aux]][3]);
                        minime=min(minime,d);
                        if(!d)
                            aux=5;
                        aux++;
                    }
                    if(aux==5){
                        best[++puncte][0]=x;
                        best[puncte][1]=debris[d1][1];
                        best[puncte][2]=z;
                        best[puncte][3]=minime;
                    }
                }
            maxim=best[1][3],pozmax=1;
            for(aux=2; aux<=puncte; aux++)
                if(maxim<best[aux][3]){
                    maxim=best[aux][3];
                    pozmax=aux;
                }
            scnt++;
            spots[scnt].coords[0]=best[pozmax][0];
            spots[scnt].coords[1]=best[pozmax][1];
            spots[scnt].coords[2]=best[pozmax][2];
            spots[scnt].fit=(puncte>0);
        }
}

void afis(){
    for(int i=1; i<=22; i++)
        DEBUG(("%d : %f %f %f %d\n",i,spots[i].coords[0],spots[i].coords[1],spots[i].coords[2],spots[i].fit));
}

void fast(){
    int damage[4]= {0,10,25,50};
    int green[10];
    int purple[10];
    int orange[5];

    /// green[x]={1, daca sfera nu poate trece pe acolo| 0, daca poate
    /// purple[x]={1, daca sfera nu poate trece pe acolo| 0, daca poate
    /// orange[x]={1, daca sfera nu poate trece pe acolo| 0, daca poate
    int vpurple[10][10]= {{0},{4,5,2,4,1},{6,5,2,4,6,1,3},{4,5,2,6,3},{6,5,2,4,8,1,7},{9,5,2,4,6,8,1,3,7,9},{6,5,2,6,8,3,9},{4,5,4,8,7},{6,5,4,6,8,7,9},{4,5,6,8,9}};
    int vorange[5][4]= {{0},{5,2,4,1},{5,2,6,3},{5,4,8,7},{5,6,8,9}};
    int ord[10]= {0,5,2,4,6,8,1,3,7,9};
    int b[2][10];

    struct chiarnustiu{
        int anterior;
        int nume;
        int cost;
    } q[1001];
    int i,j,size,cnt,minim,imin,BIG=2001;
    minim=BIG;
    for(i=1; i<=9; i++)
        b[0][i]=BIG;
    for(i=1; i<=9; i++)
        green[i]=damage[1]*(!spots[i].fit);
    for(i=1; i<=9; i++)
        purple[i]=damage[2]*(!spots[i+9].fit);
    for(i=1; i<=4; i++)
        orange[i]=damage[3]*(!spots[i+18].fit);
    cnt=0,size=0;
    for(i=1; i<=4; i++){
        if(orange[i]==0){
            cnt++;
            q[++size].nume=i;
            q[size].anterior=0;
            q[size].cost=0;
        }
    }
    for(i=1; i<=cnt; i++)
        for(j=0; j<4; j++){
            q[++size].nume=vorange[q[i].nume][j];
            q[size].anterior=i;
            q[size].cost=purple[q[size].nume];
        }
    cnt=size;
    for(; i<=cnt; i++){
        for(j=1; j<=vpurple[q[i].nume][0]; j++){
            q[++size].nume=vpurple[q[i].nume][j];
            q[size].anterior=i;
            q[size].cost=q[i].cost+green[q[size].nume];
            if(q[size].cost<b[0][q[size].nume]){
                b[0][q[size].nume]=q[size].cost;
                b[1][q[size].nume]=q[size].anterior;
            }
        }
    }
    minim=b[0][ord[1]], imin=ord[1];
    for(i=2; i<=9; i++)
        if(b[0][ord[i]]<minim){
            minim=b[0][ord[i]];
            imin=ord[i];
        }
    ansgreen=imin;
    anspurple=q[b[1][imin]].nume+9;
    ansorange=q[q[b[1][imin]].anterior].nume+18;
}

bool toDestination(float x, float y, float z){
    api.getMyZRState(myState);
    float distance=(myState[0]-x)*(myState[0]-x)+(myState[1]-y)*(myState[1]-y)+(myState[2]-z)*(myState[2]-z);
    distance=sqrtf(distance);
    if(distance<=arrive)
        return true;
    return false;
}
void loop()
{
    api.getOtherSphState(angulo);
    api.getMyZRState(myState);
    vel1[0] = (myState[3]*0.78);
    vel1[1] = -0.045;
    vel1[2] = (myState[5]*0.78);
    vel2[0] = 0;
    vel2[1] = -0.010;
    vel2[2] = 0;
    vel3[0] = (0);
    vel3[1] = 0.99;
    vel3[2] = (0);
    vel4[0] = (0);
    vel4[1] = 0.99;
    vel4[2] = (0);
    vel5[0] = 0;
    vel5[1] = (myState[4]*-1);
    vel5[2] = 0;
    vel6[0] = 0.9;
    vel6[1] = 0.9;
    vel6[2] = 0;
    api.getMyZRState(myState);
    api.getOtherSphState(angulo);
    t = game.getGamePhase();
    loop_cnt=api.getTime();

    if(t==1){//debris part
        if(loop_cnt==0){
            game.getDebris(debris);
            solve(1,9,4,0,2,1,4);
            solve(17,25,4,0,2,1,4);
            solve(33,36,3,0,1,1,3);
            afis();
            fast();
            DEBUG(("%d %d %d\n",ansgreen,anspurple,ansorange));
            A.reach=false;
            A.coords[0]=spots[ansgreen].coords[0];
            A.coords[1]=0.75f;
            A.coords[2]=spots[ansgreen].coords[2];
            B.reach=false;
            B.coords[0]=spots[ansgreen].coords[0];
            B.coords[1]=spots[ansgreen].coords[1];
            B.coords[2]=spots[ansgreen].coords[2];
            C.reach=false;
            C.coords[0]=spots[anspurple].coords[0];
            C.coords[1]=spots[anspurple].coords[1];
            C.coords[2]=spots[anspurple].coords[2];
            D.reach=false;
            D.coords[0]=spots[ansorange].coords[0];
            D.coords[1]=spots[ansorange].coords[1];
            D.coords[2]=spots[ansorange].coords[2];
            F.reach=false;
            F.coords[0]=spots[ansorange].coords[0];
            F.coords[1]=0.1f;
            F.coords[2]=spots[ansorange].coords[2];
            p1[0] = myState[0];
            p1[1] = -7;
            p1[2] = myState[2];
        }
        else{
            if(A.reach==false){
                api.setPositionTarget(A.coords);
                A.reach=toDestination(A.coords[0],A.coords[1],A.coords[2]);
                api.setVelocityTarget(vel3);
            }
            if(B.reach==false){
                api.setPositionTarget(B.coords);
                B.reach=toDestination(B.coords[0],B.coords[1],B.coords[2]);
                api.setVelocityTarget(vel3);
            }
            if(C.reach==false){
                api.setPositionTarget(C.coords);
                C.reach=toDestination(C.coords[0],C.coords[1],C.coords[2]);
                api.setVelocityTarget(vel3);
            }
            if(D.reach==false){
                api.setPositionTarget(D.coords);
                D.reach=toDestination(D.coords[0],D.coords[1],D.coords[2]);
                api.setVelocityTarget(vel4);
            }
            if(F.reach==false){
                api.setPositionTarget(F.coords);
                F.reach=toDestination(F.coords[0],F.coords[1],F.coords[2]);
                api.setVelocityTarget(vel1);
            }
        }
    }
//fase 2, Rendezvous
    if(t == 2){
        if (loop_cnt<25){
            if(myState[0]<-0.1){
                api.setPositionTarget(p7);
                api.getOtherSphState(angulo);
                quat[0] = -0.5;
                quat[1] = 0.5;
                quat[2] = -0.5;
                quat[3] = 0.5;
                api.setQuatTarget((quat));
                vel6[0] = 0.9;
                vel6[1] = 0.5;
                vel6[2] = 0;
                api.setVelocityTarget(vel6);
                check = game.checkRendezvous();
                if(check == true)
                    game.completeRendezvous();

            }
            if(myState[0]<0.25 && myState[0]>-0.1){
                api.setPositionTarget(p7);
                api.getOtherSphState(angulo);
                quat[0] = -0.5;
                quat[1] = 0.5;
                quat[2] = -0.5;
                quat[3] = 0.5;
                api.setQuatTarget((quat));
                vel6[0] = 0;
                vel6[1] = 0.55;
                vel6[2] = 0;
                api.setVelocityTarget(vel6);
                check = game.checkRendezvous();
                if(check == true)
                    game.completeRendezvous();

            }
            if(myState[0]>0.25){
                api.setPositionTarget(p7);
                api.getOtherSphState(angulo);
                quat[0] = -0.5;
                quat[1] = 0.5;
                quat[2] = -0.5;
                quat[3] = 0.5;
                api.setQuatTarget((quat));
                vel6[0] = -0.1;
                vel6[1] = 0.50;
                vel6[2] = 0;
                api.setVelocityTarget(vel6);
                check = game.checkRendezvous();
                if(check == true)
                    game.completeRendezvous();

            }
            if(myState[2]>0.28){
                api.setPositionTarget(p7);
                api.getOtherSphState(angulo);
                quat[0] = -0.5;
                quat[1] = 0.5;
                quat[2] = -0.5;
                quat[3] = 0.5;
                api.setQuatTarget((quat));
                vel6[0] = 0;
                vel6[1] = 0.55;
                vel6[2] = -0.4;
                api.setVelocityTarget(vel6);
                check = game.checkRendezvous();
                if(check == true)
                    game.completeRendezvous();

            }
            if(myState[2]<-0.25){
                api.setPositionTarget(p7);
                api.getOtherSphState(angulo);
                quat[0] = -0.5;
                quat[1] = 0.5;
                quat[2] = -0.5;
                quat[3] = 0.5;
                api.setQuatTarget((quat));
                vel6[0] = 0;
                vel6[1] = 0.55;
                vel6[2] = 0.2;
                api.setVelocityTarget(vel6);
                check = game.checkRendezvous();
                if(check == true)
                    game.completeRendezvous();

            }
        }
        else{
            api.getOtherSphState(angulo);
            quat[0] = -0.5;
            quat[1] = 0.5;
            quat[2] = -0.5;
            quat[3] = 0.5;
            p8[0] = 0.2;
            p8[1] = -0.160;
            p8[2] = -0.02;
            api.setQuatTarget((quat));
            api.setPositionTarget(p8);
            check = game.checkRendezvous();
            if(check == true)
                game.completeRendezvous();

        }
    }
    //phase 3
    if(t == 3){
        if (loop_cnt < 53){
            api.getOtherSphState(angulo);
            quat[0] = angulo[6];
            quat[1] = angulo[7]*-1;
            quat[2] = angulo[8];
            quat[3] = angulo[9]*-1;


            p8[0] = angulo[0]+0.2;
            p8[1] = angulo[1]+0.335;
            p8[2] = angulo[2]+0.01;
            api.setQuatTarget((quat));
            rota[0] = angulo[10];
            rota[1] = angulo[11];
            rota[2] = angulo[12];
            sphvel[0] = 0;
            sphvel[1] = 0.00001;
            sphvel[2] = 0.01;
            api.setAttRateTarget(rota);
            api.setVelocityTarget(sphvel);
            api.setPositionTarget(p8);
        }
        if (loop_cnt > 55){
            api.getOtherSphState(angulo);
            quat[0] = angulo[6];
            quat[1] = angulo[7]*-1;
            quat[2] = angulo[8];
            quat[3] = angulo[9]*-1;
            p8[0] = angulo[0]+0.0001;
            p8[1] = angulo[1]+0.3328;
            p8[2] = angulo[2]+0.005;
            api.setQuatTarget((quat));
            api.setPositionTarget(p8);
            rota[0] = angulo[10];
            rota[1] = angulo[11];
            rota[2] = angulo[12];
            sphvel[0] = 0;
            sphvel[1] = 0.00001;
            sphvel[2] = 0;
            api.setAttRateTarget(rota);
            api.setVelocityTarget(sphvel);
        }
    }
    //phase 4
    if (t == 4){
        if (angulo[1] < -0.3){
            p8[0] = angulo[0]-0.000;
            p8[1] = angulo[1]+0.331000;
            p8[2] = angulo[2]+0.000;
            p6[0] = p8[0];
            p6[1] = 0.7;
            p6[2] = p8[2];
            quat[0] = angulo[6];
            quat[1] = angulo[7]*-1;
            quat[2] = angulo[8];
            quat[3] = angulo[9]*-1;
            api.setQuatTarget((quat));
            api.setPositionTarget(p6);
            rota[0] = angulo[10];
            rota[1] = angulo[11];
            rota[2] = angulo[12];
            api.setAttRateTarget(rota);
        }
        else if(angulo[1] >= -0.3){
            p8[0] = angulo[0]-0.000;
            p8[1] = angulo[1]+0.331000;
            p8[2] = angulo[2]+0.000;
            quat[0] = angulo[8]*-1;
            quat[1] = angulo[9]*-1;
            quat[2] = angulo[6]*-1;
            quat[3] = angulo[7]*-1;
            p6[0] = p8[0]+5;
            p6[1] = 0.7;
            p6[2] = p8[2]-5;
            api.setQuatTarget(quat);
            api.setPositionTarget(p6);
            rota[0] = angulo[10];
            rota[1] = angulo[11];
            rota[2] = angulo[12];
            api.setAttRateTarget(rota);
        }
    }
    loop_cnt++;
}
