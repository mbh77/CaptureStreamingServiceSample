cbuffer Constants : register(b0)
{
    uint2 FrameSize;
    uint2 OutputSize;
};

Texture2D<float4> InputTexture : register(t0);
SamplerState InputSamplerState : register(s1);
RWTexture2D<uint> OutputY : register(u0);
RWTexture2D<uint> OutputU : register(u1);
RWTexture2D<uint> OutputV : register(u2);

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint2 pos = DTid.xy;

    if (pos.x >= OutputSize.x || pos.y >= OutputSize.y)
        return;

    float widthRatio = (float) OutputSize.x / FrameSize.x;
    float heightRatio = (float) OutputSize.y / FrameSize.y;

    uint scaledWidth, scaledHeight;
    if (widthRatio < heightRatio)
    {
        scaledWidth = OutputSize.x;
        scaledHeight = (uint) (FrameSize.y * widthRatio);
    }
    else
    {
        scaledWidth = (uint) (FrameSize.x * heightRatio);
        scaledHeight = OutputSize.y;
    }
    
    uint2 padLeftTop = uint2((OutputSize.x - scaledWidth) / 2, (OutputSize.y - scaledHeight) / 2);

    // 레터박스
    if (pos.x < padLeftTop.x || pos.x >= padLeftTop.x + scaledWidth ||
        pos.y < padLeftTop.y || pos.y >= padLeftTop.y + scaledHeight)
    {
        // 레터박스 영역 (검정색 Y = 0, 중립값 U = 128, V = 128)
        OutputY[pos] = 0;
        if (pos.x % 2 == 0 && pos.y % 2 == 0)
        {
            uint2 uvPos = pos / 2;
            OutputU[uvPos] = 128;
            OutputV[uvPos] = 128;
        }
        return;
    }
    
    float2 uv = float2(
        (float) (pos.x - padLeftTop.x) / scaledWidth,
        (float) (pos.y - padLeftTop.y) / scaledHeight
    );
    
    float4 bgra = InputTexture.SampleLevel(InputSamplerState, uv, 0);
    float r = saturate(bgra.r);
    float g = saturate(bgra.g);
    float b = saturate(bgra.b);
    
    // YUV 변환 (BT.709)
    uint y = (uint) ((0.183f * r + 0.614f * g + 0.062f * b) * 255.0f) + 16;
    OutputY[pos] = y;
    
    // UV는 2x2 블록의 평균값 계산
    if (pos.x % 2 == 0 && pos.y % 2 == 0)
    {
        // 2x2 블록의 기준 좌표 (짝수 좌표)
        float uSum = 0.0f;
        float vSum = 0.0f;

        for (int yOffset = 0; yOffset < 2; ++yOffset)
        {
            for (int xOffset = 0; xOffset < 2; ++xOffset)
            {
                float2 offsetUV = float2(
                    ((float) (pos.x + xOffset - padLeftTop.x) / scaledWidth),
                    ((float) (pos.y + yOffset - padLeftTop.y) / scaledHeight)
                );

                float4 offsetBgra = InputTexture.SampleLevel(InputSamplerState, offsetUV, 0);
                float offsetR = offsetBgra.r;
                float offsetG = offsetBgra.g;
                float offsetB = offsetBgra.b;

                // U, V 값 계산 및 누적
                uSum += (-0.1006f * offsetR - 0.3386f * offsetG + 0.4392f * offsetB);
                vSum += (0.4392f * offsetR - 0.3989f * offsetG - 0.0403f * offsetB);
            }
        }

        // 2x2 블록의 평균값 계산
        uint u = (uint) (((uSum / 4.0f) * 255.0f) + 128.0f);
        uint v = (uint) (((vSum / 4.0f) * 255.0f) + 128.0f);

        // UV 좌표 저장
        uint2 uvPos = pos / 2;
        if (uvPos.x < (OutputSize.x / 2) && uvPos.y < (OutputSize.y / 2))
        {
            OutputU[uvPos] = u;
            OutputV[uvPos] = v;
        }
    }
}