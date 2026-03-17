#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H

#pragma once

#include <Malena/Graphics/Base/Graphic.h>
#include <Malena/Traits/Customizable.h>

namespace ml
{
    /**
     * @brief Framework-integrated vertex array. (Currently a stub — not yet implemented.)
     *
     * @c VertexArray is intended to wrap @c sf::VertexArray inside
     * @c Graphic<> in the same way @c Circle and @c Rectangle do, giving
     * it @c Subscribable, @c Flaggable, @c Positionable, and @c Draggable
     * behavior.
     *
     * The implementation is currently commented out pending resolution of
     * the @c Graphic<sf::VertexArray> specialization (vertex arrays have no
     * @c getGlobalBounds(), so the fallback in @c DrawableWrapper — returning
     * the full window rect — would need to be applied here as well).
     *
     * @note Do not use this class yet. Use @c sf::VertexArray directly for
     *       custom geometry until this wrapper is completed.
     *
     * @see Graphic, DrawableWrapper
     */
    // class VertexArray : public Core<sf::VertexArray> { ... };
    // template<typename MANIFEST>
    // class VertexArrayWith : public Customizable<VertexArray, MANIFEST>{};

} // namespace ml

#endif // VERTEXARRAY_H
