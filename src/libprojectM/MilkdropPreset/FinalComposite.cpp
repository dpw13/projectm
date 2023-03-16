#include "FinalComposite.hpp"

#include "PresetState.hpp"

#include <cstddef>

static std::string const defaultCompositeShader = "shader_body\n{\nret = tex2D(sampler_main, uv).xyz;\n}";

FinalComposite::FinalComposite()
    : RenderItem()
{
    RenderItem::Init();
}

void FinalComposite::InitVertexAttrib()
{
    glGenBuffers(1, &m_elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBuffer);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), nullptr);                                               // Positions
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), reinterpret_cast<void*>(offsetof(MeshVertex, r)));      // Colors
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), reinterpret_cast<void*>(offsetof(MeshVertex, u)));      // Textures
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), reinterpret_cast<void*>(offsetof(MeshVertex, radius))); // Radius/Angle
}

void FinalComposite::LoadCompositeShader(const PresetState& presetState)
{
    if (presetState.compositeShaderVersion > 0)
    {
        m_compositeShader = std::make_unique<MilkdropShader>(MilkdropShader::ShaderType::CompositeShader);
        if (!presetState.warpShader.empty())
        {
            try
            {
                m_compositeShader->LoadCode(presetState.compositeShader);
#ifdef MILKDROP_PRESET_DEBUG
                std::cerr << "[Composite Shader] Loaded composite shader code." << std::endl;
#endif
            }
            catch (ShaderException& ex)
            {
#ifdef MILKDROP_PRESET_DEBUG
                std::cerr << "[Composite Shader] Error loading composite warp shader code:" << ex.message() << std::endl;
                std::cerr << "[Composite Shader] Using fallback shader." << std::endl;
#endif
                // Fall back to default shader
                m_compositeShader = std::make_unique<MilkdropShader>(MilkdropShader::ShaderType::CompositeShader);
                m_compositeShader->LoadCode(defaultCompositeShader);
            }
        }
        else
        {
            m_compositeShader->LoadCode(defaultCompositeShader);
#ifdef MILKDROP_PRESET_DEBUG
            std::cerr << "[Composite Shader] Loaded default composite shader code." << std::endl;
#endif
        }
    }
}

void FinalComposite::CompileCompositeShader(PresetState& presetState)
{
    if (m_compositeShader)
    {
        try
        {
            m_compositeShader->LoadTexturesAndCompile(presetState);
#ifdef MILKDROP_PRESET_DEBUG
            std::cerr << "[Composite Shader] Successfully compiled composite shader code." << std::endl;
#endif
        }
        catch (ShaderException& ex)
        {
#ifdef MILKDROP_PRESET_DEBUG
            std::cerr << "[Composite Shader] Error compiling composite warp shader code:" << ex.message() << std::endl;
            std::cerr << "[Composite Shader] Using fallback shader." << std::endl;
#endif
            // Fall back to default shader
            m_compositeShader = std::make_unique<MilkdropShader>(MilkdropShader::ShaderType::CompositeShader);
            m_compositeShader->LoadCode(defaultCompositeShader);
            m_compositeShader->LoadTexturesAndCompile(presetState);
        }
    }
}

void FinalComposite::Draw(const PresetState& presetState, const PerFrameContext& perFrameContext)
{
    InitializeMesh(presetState);
}

void FinalComposite::InitializeMesh(const PresetState& presetState)
{
    if (m_viewportWidth == presetState.renderContext.viewportSizeX &&
        m_viewportHeight == presetState.renderContext.viewportSizeY)
    {
        return;
    }

    float const dividedByX = 1.0f / static_cast<float>(compositeGridWidth - 2);
    float const dividedByY = 1.0f / static_cast<float>(compositeGridHeight - 2);
    
    constexpr float PI = 3.1415926535898f;

    for (int gridY = 0; gridY < compositeGridHeight; gridY++)
    {
        int const gridY2 = gridY - gridY / (compositeGridHeight / 2);
        float v = gridY2 * dividedByY;
        v = SquishToCenter(v, 3.0f);
        float sy = -((v) *2 - 1);
        for (int gridX = 0; gridX < compositeGridWidth; gridX++)
        {
            int i2 = gridX - gridX / (compositeGridWidth / 2);
            float u = i2 * dividedByX;
            u = SquishToCenter(u, 3.0f);
            float sx = (u) *2 - 1;
            auto& pComp = m_vertices.at(gridX + gridY * compositeGridWidth);
            pComp.x = sx;
            pComp.y = sy;

            float rad;
            float ang;
            UvToMathSpace(presetState.renderContext.aspectX,
                          presetState.renderContext.aspectY,
                          u, v, rad, ang);
            
            // fix-ups:
            if (gridX == compositeGridWidth / 2 - 1)
            {
                if (gridY < compositeGridHeight / 2 - 1) {
                    ang = PI * 1.5f;
                } else if (gridY == compositeGridHeight / 2 - 1)
                {
                    ang = PI * 1.25f;
                }
                else if (gridY == compositeGridHeight / 2) {
                    ang = PI * 0.75f;
                } else
                {
                    ang = PI * 0.5f;
                }
            }
            else if (gridX == compositeGridWidth / 2)
            {
                if (gridY < compositeGridHeight / 2 - 1)
                {
                    ang = PI * 1.5f;
                }
                else if (gridY == compositeGridHeight / 2 - 1)
                {
                    ang = PI * 1.75f;
                }
                else if (gridY == compositeGridHeight / 2)
                {
                    ang = PI * 0.25f;
                }
                else
                {
                    ang = PI * 0.5f;
                }
            }
            else if (gridY == compositeGridHeight / 2 - 1)
            {
                if (gridX < compositeGridWidth / 2 - 1)
                {
                    ang = PI * 1.0f;
                }
                else if (gridX == compositeGridWidth / 2 - 1)
                {
                    ang = PI * 1.25f;
                }
                else if (gridX == compositeGridWidth / 2)
                {
                    ang = PI * 1.75f;
                }
                else
                {
                    ang = PI * 2.0f;
                }
            }
            else if (gridY == compositeGridHeight / 2)
            {
                if (gridX < compositeGridWidth / 2 - 1)
                {
                    ang = PI * 1.0f;
                }
                else if (gridX == compositeGridWidth / 2 - 1)
                {
                    ang = PI * 0.75f;
                }
                else if (gridX == compositeGridWidth / 2)
                {
                    ang = PI * 0.25f;
                }
                else
                {
                    ang = PI * 0.0f;
                }
            }
            pComp.u = u;
            pComp.v = v;

            pComp.radius = rad;
            pComp.angle = ang;
        }
    }

    // build index list for final composite blit -
    // order should be friendly for interpolation of 'ang' value!
    int currentIndex = 0;
    for (int gridY = 0; gridY < compositeGridHeight - 1; gridY++)
    {
        if (gridY == compositeGridHeight / 2 - 1)
        {
            continue;
        }

        for (int gridX = 0; gridX < compositeGridWidth - 1; gridX++)
        {
            if (gridX == compositeGridWidth / 2 - 1)
            {
                continue;
            }

            bool const leftHalf = (gridX < compositeGridWidth / 2);
            bool const topHalf = (gridY < compositeGridHeight / 2);
            bool const center4 = ((gridX == compositeGridWidth / 2 || gridX == compositeGridWidth / 2 - 1) &&
                                  (gridY == compositeGridHeight / 2 || gridY == compositeGridHeight / 2 - 1));

            if ((static_cast<int>(leftHalf) + static_cast<int>(topHalf) + static_cast<int>(center4)) % 2 == 1)
            {
                m_indices[currentIndex + 0] = gridY * compositeGridWidth + gridX;
                m_indices[currentIndex + 1] = gridY * compositeGridWidth + gridX + 1;
                m_indices[currentIndex + 2] = (gridY + 1) * compositeGridWidth + gridX + 1;
                m_indices[currentIndex + 3] = (gridY + 1) * compositeGridWidth + gridX + 1;
                m_indices[currentIndex + 4] = (gridY + 1) * compositeGridWidth + gridX;
                m_indices[currentIndex + 5] = gridY * compositeGridWidth + gridX;
            }
            else
            {
                m_indices[currentIndex + 0] = (gridY + 1) * compositeGridWidth + (gridX);
                m_indices[currentIndex + 1] = (gridY) *compositeGridWidth + (gridX);
                m_indices[currentIndex + 2] = (gridY) *compositeGridWidth + (gridX + 1);
                m_indices[currentIndex + 3] = (gridY) *compositeGridWidth + (gridX + 1);
                m_indices[currentIndex + 4] = (gridY + 1) * compositeGridWidth + (gridX + 1);
                m_indices[currentIndex + 5] = (gridY + 1) * compositeGridWidth + (gridX);
            }

            currentIndex += 6;
        }
    }

    // Store indices.
    // ToDo: Probably don't need to store
    glBindVertexArray(m_vaoID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * m_indices.size(), m_indices.data(), GL_STATIC_DRAW);
    glBindVertexArray(0);
}

float FinalComposite::SquishToCenter(float x, float exponent)
{
    if (x > 0.5f)
    {
        return powf(x * 2.0f - 1.0f, exponent) * 0.5f + 0.5f;
    }

    return (1.0f - powf(1.0f - x * 2.0f, exponent)) * 0.5f;
}

void FinalComposite::UvToMathSpace(float aspectX, float aspectY,
                                   float u, float v, float& rad, float& ang)
{
    // (screen space = -1..1 on both axes; corresponds to UV space)
    // uv space = [0..1] on both axes
    // "math" space = what the preset authors are used to:
    //      upper left = [0,0]
    //      bottom right = [1,1]
    //      rad == 1 at corners of screen
    //      ang == 0 at three o'clock, and increases counter-clockwise (to 6.28).
    float const px = (u * 2.0f - 1.0f) * aspectX; // probably 1.0
    float const py = (v * 2.0f - 1.0f) * aspectY; // probably <1

    rad = sqrtf(px * px + py * py) / sqrtf(aspectX * aspectX + aspectY * aspectY);
    ang = atan2f(py, px);
    if (ang < 0)
    {
        ang += 6.2831853071796f;
    }
}