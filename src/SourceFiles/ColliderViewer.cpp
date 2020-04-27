#include "ColliderViewer.h"
#include "Resources.h"

ColliderViewer::ColliderViewer() :
    Viewer(Resources::Transparent) {
}

void ColliderViewer::init() {
	Viewer::init();
	renderer_ = SDL_Game::instance()->getRenderer();
	points_ = new SDL_Point[5];
    body_ = collider_->getBody();
    int layer = collider_->getCollisionLayer();
    if(layer > 0 && layer < 256)
        color = colors[(int)log2(layer)];
}

void ColliderViewer::drawRect(b2Fixture* fixture) const {
    setPoints(fixture);
    SDL_RenderDrawLines(renderer_, points_, 5);
}

void ColliderViewer::setPoints(b2Fixture* fixture) const {

    SDL_Rect rect = collider_->getRectRender();
    float angle = -collider_->getAngle();
    float diagonalLength = sqrt(rect.w * rect.w + rect.h * rect.h);

	points_[0].x = rect.x;
	points_[0].y = rect.y;

	points_[1].x = rect.x + rect.w * cos(angle);
	points_[1].y = rect.y + rect.w * sin(angle);

	points_[2].x = points_[1].x + rect.h * cos(90 + angle);
	points_[2].y = points_[1].y + rect.h * sin(90 + angle);

	points_[3].x = rect.x + rect.h * cos(angle + 90);
	points_[3].y = rect.y + rect.h * sin(angle + 90);

    points_[4].x = rect.x;
    points_[4].y = rect.y;
}

void ColliderViewer::draw() const {
	if (drawable_ && !isUIElement_) {

        // lista de fixtures del body
        b2Fixture* f = body_->GetFixtureList();

        // recorre todos los fixtures del objeto
        while (f != nullptr) {

            int a = Collider::CollisionLayer::NormalAttachableObject;
            // cambia color de dibujado
            SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, 255);

            // collider circular
            if (f->GetShape()->GetType() == b2Shape::e_circle)
                drawCircle(renderer_, 0, 0, 0);

            // collider rectangular
            else drawRect(f);

            f = f->GetNext();   // siguiente fixture del body
        }
	}
}

// este algoritmo es bastante r�pido (~500 microsegundos)
void ColliderViewer::drawCircle(SDL_Renderer* renderer, int32_t centreX, int32_t centreY, int32_t radius) const
{
    const int32_t diameter = (radius * 2);

    int32_t x = (radius - 1);
    int32_t y = 0;
    int32_t tx = 1;
    int32_t ty = 1;
    int32_t error = (tx - diameter);

    while (x >= y)
    {
        SDL_RenderDrawPoint(renderer, centreX + x, centreY - y);
        SDL_RenderDrawPoint(renderer, centreX + x, centreY + y);
        SDL_RenderDrawPoint(renderer, centreX - x, centreY - y);
        SDL_RenderDrawPoint(renderer, centreX - x, centreY + y);
        SDL_RenderDrawPoint(renderer, centreX + y, centreY - x);
        SDL_RenderDrawPoint(renderer, centreX + y, centreY + x);
        SDL_RenderDrawPoint(renderer, centreX - y, centreY - x);
        SDL_RenderDrawPoint(renderer, centreX - y, centreY + x);

        if (error <= 0)
        {
            ++y;
            error += ty;
            ty += 2;
        }

        if (error > 0)
        {
            --x;
            tx += 2;
            error += (tx - diameter);
        }
    }
}
