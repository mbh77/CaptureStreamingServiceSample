cbuffer Constants : register(b0)
{
    uint2 FrameSize;
};

Texture2D<float4> InputTexture : register(t0);
RWTexture2D<float4> OutputTexture : register(u0);

[numthreads(16, 16, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint2 pos = DTid.xy;
    
    if (pos.x >= FrameSize.x || pos.y >= FrameSize.y)
    {
        return;
    }
    
    float4 bgra = InputTexture.Load(int3(pos, 0));
    
    // 알파 채널이 0인 경우 기본값 설정
    if (bgra.a == 0.0)
    {
        bgra = float4(1.0, 0.0, 1.0, 1.0); // 테스트용: 마젠타로 표시
    }
    
    OutputTexture[pos] = bgra;
}