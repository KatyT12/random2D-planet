
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "stdint.h"
#include <sstream>

#include <stdio.h>
#include <stdlib.h>

#include <string>


class Planet
{
    public:
    olc::vi2d pos;
   
    int radius;
    
    int baseColor;
    
    std::vector<float> generationChances;

    int num;

    std::vector<olc::Pixel> generationColors;
    
    int outputWidth;
    int outputHeight;
    float *fSeeds = nullptr;
    float *PerlInNoise;
    float bias;
    int nOctaves;


    olc::Pixel color;



    Planet()
    {


    }

    void init(olc::vi2d pos)
    {
        color = getRandomColor(5,255);
        radius = randomDouble(40,80);
        
        num = randomInt(4,14);

        generationColors = {};

        if(radius >= 60)
        {
            nOctaves = randomInt(5,7);
        }
        else
        {
            nOctaves = randomInt(4,6);        
        }

        //nOctaves = 3;
        
        

        outputHeight = (radius * 2);
        outputWidth = (radius * 2);
        PerlInNoise = new float[outputWidth * outputHeight];
        bias = randomDouble(1.4f,2.0f);

        fSeeds = new float[outputWidth * outputHeight];
        for(int i = 0; i < (outputWidth * outputHeight); i++)
        {
            fSeeds[i] = (float)rand() / (float)RAND_MAX;
        }
        

        for(unsigned int i = 0; i < num; i++)
        {
            generationColors.push_back(getRandomColor(4,255));
        }

        perlInNoise2D(outputWidth,outputHeight,fSeeds,nOctaves,bias,PerlInNoise);


     

        generationChances.push_back(0.5f);
    }



    void setSeed()
    {
        srand(std::chrono::duration_cast<std::chrono::nanoseconds>(
                    std::chrono::system_clock::now().time_since_epoch()
                    ).count());   
    }


    olc::Pixel getRandomColor(int min, int max)
    {
        
        setSeed();
    
      olc::Pixel p = olc::Pixel(rand() % (max-min) + min,rand() % (max-min)+min,rand() % (max-min)+min); 
       
      return p;

    }
    double randomDouble(double min, double max)
    {
        setSeed();
        return ((max - min) * ((float)rand() / RAND_MAX)) + min;
    }
    int randomInt(int min, int max)
    {
        setSeed();
        return rand() % (max - min) + min;
    }

    olc::Pixel getColor(int x, int y)
    {
       
        

        float col = PerlInNoise[y * outputWidth + x];
        
        olc::Pixel color;
        /*
        if(col < 0.25f)
        {
            color = generationColors[0];
        }
        else if(col >= 0.25f && col < 0.5f)
        {
            color = generationColors[1];
        }
        else if(col >= 0.5f  && col < 0.75f)
        {
            color = generationColors[2];
        }
        else if(col >= 0.75f)
        {
            color = generationColors[3];
        }
        */
        color = pickColor(col);
        return color;
    }


    olc::Pixel pickColor(float col)
    {
        float offset = 1.0f / generationColors.size();
        
        float min = 0.0f;
        float max = offset;
        
        olc::Pixel ret;
        for(unsigned int i = 0; i < generationColors.size(); i++)
        {
            if(col >= min && col < max)
            {
                return generationColors[i];
            }
            min += offset;
            max += offset;
        }
        return olc::WHITE;



    }



    void perlInNoise2D(int nWidth,int nHeight, float *Seed, int nOctaves, float fBias, float *fOutput)
    {
        //Use 1D perlin noise
        for(int x = 0; x < nWidth; x++)
        {
            for(int y = 0; y < nHeight; y++)
            {

                float fNoise = 0.0f;
                float fScale = 1.0f;
                float fScaleAccum = 0.0f;
                for(int o = 0; o < nOctaves;o++)
                {

                    //Instead of nCount will be for our width because of our noise arrays always being powers of 2 and our pitching and sampling periods always wanting to be powers of 2 this is ok
                    

                    int nPitch = nWidth >> o; 
                    

                    //Instead of setting two sample points, will set sample points in the x and y axis
                    int sampleX1 = (x / nPitch) * nPitch;
                    int sampleY1 = (y / nPitch) * nPitch;

                    //and we will pick 2 samples because we will need to linearly interpolate between them

                    int sampleX2 = (sampleX1 + nPitch) % nWidth;
                    int sampleY2 = (sampleY1 + nPitch) % nWidth;


                    //Instead of a single blend we now need a blend in the x and y axis 
                    float fBlendX = (float)(x - sampleX1) / (float) nPitch;
                    float fBlendY = (float)(y - sampleY1) / (float) nPitch;


                    //And will pick 2 samples because we need to now linearly interpolate between them
                    
                    //We are effectively taking two slices of 1D perlin noise so we use fBlendX twice
                    float fSampleT = (1.0f - fBlendX) * Seed[sampleY1 * nWidth + sampleX1] + fBlendX * Seed[sampleY1 * nWidth + sampleX2];
                    float fSampleB = (1.0f - fBlendX) * Seed[sampleY2 * nWidth + sampleX1] + fBlendX * Seed[sampleY2 * nWidth + sampleX2];


                    //BlendY is used to linearly interpolate between the two samples
                    fNoise += (fBlendY * (fSampleB - fSampleT) + fSampleT) * fScale;

                    fScaleAccum += fScale;

                    fScale = fScale / fBias;
                    }


                fOutput[(y * nWidth) + x] = fNoise / fScaleAccum;
                //std::cout << fOutput[y * nWidth + x] << "\n";

                }
            }

        }








};




class Example : public olc::PixelGameEngine
{
public:
	Example()
	{
        
    	sAppName = "Example";
     

	}
     
    olc::vi2d pos = {ScreenWidth()/2,ScreenHeight()/2};
    Planet p;
    

public:
	bool OnUserCreate() override
	{
        //Planet p(pos);
        p.init(pos);
        return true;
	}


   
    


	bool OnUserUpdate(float fElapsedTime) override
	{

        Clear(olc::BLACK);
            /*
            for (int x = 0; x < ScreenWidth(); x++)
            {
                for (int y = 0; y < ScreenHeight(); y++)
                {
                    int tmpX = (x - pos.x);
                    int tmpY = (y - pos.y);

                    //Use circle equations to determine if pixel is within circle
                    if(((tmpX * tmpX) + (tmpY * tmpY)) < p.radius*p.radius)
                    {
                       
                        Draw(x,y,p.color);
                        
                    }
                }
            }*/

        
            for(int x = -p.radius;x<p.radius;x++)
            {
                for(int y= -p.radius;y<p.radius;y++)
                {
                    if((x*x)+ (y*y) < p.radius*p.radius)
                    {
                        int tX = x + p.radius;
                        int tY = y + p.radius;
                        Draw(tX + 100,tY+100,p.getColor(tX,tY));    
                    }
                }
            }
			
        if(GetKey(olc::ENTER).bPressed)
        {
            p.init(pos);
        }


		return true;
	}


};


int main()
{
	Example demo;
	if (demo.Construct(500, 500, 4, 4))
		demo.Start();

	return 0;
}
