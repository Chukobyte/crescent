#include "curve_editor.h"

// TODO: Refactor once minimal functionality is met...

static const float NODE_SLOT_RADIUS = 4.0f;

ImVec2 operator+(const ImVec2 &thisVector, const ImVec2 &otherVector) {
    return { thisVector.x + otherVector.x, thisVector.y + otherVector.y };
}

ImVec2& operator+=(ImVec2 &thisVector, const ImVec2 &otherVector) {
    thisVector.x += otherVector.x;
    thisVector.y += otherVector.y;
    return thisVector;
}

ImVec2 operator-(const ImVec2 &thisVector, const ImVec2 &otherVector) {
    return { thisVector.x - otherVector.x, thisVector.y - otherVector.y };
}

ImVec2& operator-=(ImVec2 &thisVector, const ImVec2 &otherVector) {
    thisVector.x -= otherVector.x;
    thisVector.y -= otherVector.y;
    return thisVector;
}

ImVec2 operator*(const ImVec2 &thisVector, const ImVec2 &otherVector) {
    return { thisVector.x * otherVector.x, thisVector.y * otherVector.y };
}

ImVec2& operator*=(ImVec2 &thisVector, const ImVec2 &otherVector) {
    thisVector.x *= otherVector.x;
    thisVector.y *= otherVector.y;
    return thisVector;
}

ImVec2 operator*(const ImVec2 &thisVector, float value) {
    return { thisVector.x * value, thisVector.y * value };
}

ImVec2 operator/(const ImVec2 &thisVector, const ImVec2 &otherVector) {
    return { thisVector.x / otherVector.x, thisVector.y / otherVector.y };
}

ImVec2 operator/(const ImVec2 &thisVector, float value) {
    return { thisVector.x / value, thisVector.y / value };
}

bool operator==(const ImVec2 &thisVector, const ImVec2 &otherVector) {
    return thisVector.x == otherVector.x && thisVector.y == otherVector.y;
}

bool operator!=(const ImVec2 &thisVector, const ImVec2 &otherVector) {
    return !(thisVector == otherVector);
}

enum class StorageValues : ImGuiID {
    FROM_X = 100,
    FROM_Y,
    WIDTH,
    HEIGHT,
    IS_PANNING,
    POINT_START_X,
    POINT_START_Y
};

namespace Curve {
ImVec2 Transform(const ImVec2& pos, float from_x, float from_y, float width, float height, const ImRect& inner_bb) {
    float x = (pos.x - from_x) / width;
    float y = (pos.y - from_y) / height;

    return {
        inner_bb.Min.x * (1 - x) + inner_bb.Max.x * x,
        inner_bb.Min.y * y + inner_bb.Max.y * (1 - y)
    };
}

ImVec2 InvTransform(const ImVec2& pos, float from_x, float from_y, float width, float height, const ImRect& inner_bb) {
    float x = (pos.x - inner_bb.Min.x) / (inner_bb.Max.x - inner_bb.Min.x);
    float y = (inner_bb.Max.y - pos.y) / (inner_bb.Max.y - inner_bb.Min.y);

    return {
        from_x + width * x,
        from_y + height * y
    };
}

bool HandlePoint(ImVec2& p, float from_x, float from_y, float width, float height, const ImRect& inner_bb, int& hovered_idx, int point_idx, int idx) {
    static const float SIZE = 3;

    ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
    ImVec2 pos = Transform(p, from_x, from_y, width, height, inner_bb);

    ImGui::SetCursorScreenPos(pos - ImVec2(SIZE, SIZE));
    ImGui::PushID(idx);
    ImGui::InvisibleButton("", ImVec2(2 * NODE_SLOT_RADIUS, 2 * NODE_SLOT_RADIUS));

    ImU32 col = ImGui::IsItemActive() || ImGui::IsItemHovered() ? ImGui::GetColorU32(ImGuiCol_PlotLinesHovered) : ImGui::GetColorU32(ImGuiCol_PlotLines);

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    window->DrawList->AddLine(pos + ImVec2(-SIZE, 0), pos + ImVec2(0, SIZE), col);
    window->DrawList->AddLine(pos + ImVec2(SIZE, 0), pos + ImVec2(0, SIZE), col);
    window->DrawList->AddLine(pos + ImVec2(SIZE, 0), pos + ImVec2(0, -SIZE), col);
    window->DrawList->AddLine(pos + ImVec2(-SIZE, 0), pos + ImVec2(0, -SIZE), col);

    if (ImGui::IsItemHovered()) {
        hovered_idx = point_idx + idx;
    }

    bool changed = false;
    if (ImGui::IsItemActive() && ImGui::IsMouseClicked(0)) {
        window->StateStorage.SetFloat((ImGuiID)StorageValues::POINT_START_X, pos.x);
        window->StateStorage.SetFloat((ImGuiID)StorageValues::POINT_START_Y, pos.y);
    }

    if (ImGui::IsItemHovered() || ImGui::IsItemActive() && ImGui::IsMouseDragging(0)) {
        char tmp[64];
        ImFormatString(tmp, sizeof(tmp), "%0.2f, %0.2f", p.x, p.y);
        window->DrawList->AddText({ pos.x, pos.y - ImGui::GetTextLineHeight() }, 0xff000000, tmp);
    }

    if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0)) {
        pos.x = window->StateStorage.GetFloat((ImGuiID)StorageValues::POINT_START_X, pos.x);
        pos.y = window->StateStorage.GetFloat((ImGuiID)StorageValues::POINT_START_Y, pos.y);
        pos += ImGui::GetMouseDragDelta();
        const ImVec2 v = InvTransform(pos, from_x, from_y, width, height, inner_bb);

        p = v;
        changed = true;
    }
    ImGui::PopID();

    ImGui::SetCursorScreenPos(cursor_pos);
    return changed;
}

bool HandleTangent(ImVec2& t, const ImVec2& p, int idx, float from_x, float from_y, float width, float height, const ImRect& inner_bb, int& hovered_idx, int point_idx) {
    static const float SIZE = 2;
    static const float LENGTH = 18;

    auto normalized = [](const ImVec2& v) -> ImVec2 {
        float len = 1.0f / sqrtf(v.x *v.x + v.y * v.y);
        return { v.x * len, v.y * len };
    };

    ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
    ImVec2 pos = Transform(p, from_x, from_y, width, height, inner_bb);
    ImVec2 tang = pos + normalized(ImVec2(t.x, -t.y)) * LENGTH;

    ImGui::SetCursorScreenPos(tang - ImVec2(SIZE, SIZE));
    ImGui::PushID(-idx);
    ImGui::InvisibleButton("", ImVec2(2 * NODE_SLOT_RADIUS, 2 * NODE_SLOT_RADIUS));

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    window->DrawList->AddLine(pos, tang, ImGui::GetColorU32(ImGuiCol_PlotLines));

    ImU32 col = ImGui::IsItemHovered() ? ImGui::GetColorU32(ImGuiCol_PlotLinesHovered) : ImGui::GetColorU32(ImGuiCol_PlotLines);

    window->DrawList->AddLine(tang + ImVec2(-SIZE, SIZE), tang + ImVec2(SIZE, SIZE), col);
    window->DrawList->AddLine(tang + ImVec2(SIZE, SIZE), tang + ImVec2(SIZE, -SIZE), col);
    window->DrawList->AddLine(tang + ImVec2(SIZE, -SIZE), tang + ImVec2(-SIZE, -SIZE), col);
    window->DrawList->AddLine(tang + ImVec2(-SIZE, -SIZE), tang + ImVec2(-SIZE, SIZE), col);

    bool changed = false;
    if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0)) {
        tang = ImGui::GetIO().MousePos - pos;
        tang = normalized(tang);
        tang.y *= -1;

        t = tang;
        changed = true;
    }
    ImGui::PopID();

    ImGui::SetCursorScreenPos(cursor_pos);
    return changed;
}
} // namespace Curve

int CurveEditor(const char* label, float* values, int points_count, const ImVec2& editor_size, int flags, int* new_count) {
    const float HEIGHT = 100;
    static ImVec2 start_pan;

    ImGuiContext& context = *ImGui::GetCurrentContext();
    const ImGuiStyle& style = context.Style;
    ImVec2 size = editor_size;
    size.x = size.x < 0 ? ImGui::CalcItemWidth() + (style.FramePadding.x * 2) : size.x;
    size.y = size.y < 0 ? HEIGHT : size.y;

    ImGuiWindow* parent_window = ImGui::GetCurrentWindow();
    ImGuiID id = parent_window->GetID(label);
    if (!ImGui::BeginChildFrame(id, size, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
        ImGui::EndChild();
        return -1;
    }

    int hovered_idx = -1;
    if (new_count) {
        *new_count = points_count;
    }

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) {
        ImGui::EndChild();
        return -1;
    }

    // Calculate points min and max
    ImVec2 points_min(FLT_MAX, FLT_MAX);
    ImVec2 points_max(-FLT_MAX, -FLT_MAX);
    for (int point_idx = 0; point_idx < points_count; ++point_idx) {
        ImVec2 point;
        if (flags & (int)CurveEditorFlags::NO_TANGENTS) {
            point = ((ImVec2*)values)[point_idx];
        } else {
            point = ((ImVec2*)values)[1 + point_idx * 3];
        }
        points_max = ImMax(points_max, point);
        points_min = ImMin(points_min, point);
    }
    points_max.y = ImMax(points_max.y, points_min.y + 0.0001f);

    float from_x = window->StateStorage.GetFloat((ImGuiID)StorageValues::FROM_X, points_min.x);
    float from_y = window->StateStorage.GetFloat((ImGuiID)StorageValues::FROM_Y, points_min.y);
    float width = window->StateStorage.GetFloat((ImGuiID)StorageValues::WIDTH, points_max.x - points_min.x);
    float height = window->StateStorage.GetFloat((ImGuiID)StorageValues::HEIGHT, points_max.y - points_min.y);
    window->StateStorage.SetFloat((ImGuiID)StorageValues::FROM_X, from_x);
    window->StateStorage.SetFloat((ImGuiID)StorageValues::FROM_Y, from_y);
    window->StateStorage.SetFloat((ImGuiID)StorageValues::WIDTH, width);
    window->StateStorage.SetFloat((ImGuiID)StorageValues::HEIGHT, height);

    const ImRect inner_bb = window->InnerRect;
    const ImRect frame_bb(inner_bb.Min - style.FramePadding, inner_bb.Max + style.FramePadding);

    // Show grid
    if (flags & (int)CurveEditorFlags::SHOW_GRID) {
        int exp;
        frexp(width / 5, &exp);
        const float step_x = (float) ldexp(1.0, exp);
        const int cell_cols = int(width / step_x);

        // Horizontal Lines
        const float x = step_x * int(from_x / step_x);
        for (int i = -1; i < cell_cols + 2; ++i) {
            ImVec2 a = Curve::Transform({ x + i * step_x, from_y }, from_x, from_y, width, height, inner_bb);
            ImVec2 b = Curve::Transform({ x + i * step_x, from_y + height }, from_x, from_y, width, height, inner_bb);
            window->DrawList->AddLine(a, b, 0x55000000);
            char buf[64];
            if (exp > 0) {
                ImFormatString(buf, sizeof(buf), " %d", int(x + i * step_x));
            } else {
                ImFormatString(buf, sizeof(buf), " %f", x + i * step_x);
            }
            window->DrawList->AddText(b, 0x55000000, buf);
        }

        frexp(height / 5, &exp);
        const float step_y = (float) ldexp(1.0, exp);
        const int cell_rows = int(height / step_y);

        // Vertical Lines
        const float y = step_y * int(from_y / step_y);
        for (int i = -1; i < cell_rows + 2; ++i) {
            ImVec2 a = Curve::Transform({ from_x, y + i * step_y }, from_x, from_y, width, height, inner_bb);
            ImVec2 b = Curve::Transform({ from_x + width, y + i * step_y }, from_x, from_y, width, height, inner_bb);
            window->DrawList->AddLine(a, b, 0x55000000);
            char buf[64];
            if (exp > 0) {
                ImFormatString(buf, sizeof(buf), " %d", int(y + i * step_y));
            } else {
                ImFormatString(buf, sizeof(buf), " %f", y + i * step_y);
            }
            window->DrawList->AddText(a, 0x55000000, buf);
        }
    }

    // Zoom with mouse wheel
    if (ImGui::GetIO().MouseWheel != 0 && ImGui::IsItemHovered()) {
        const float scale = powf(2, ImGui::GetIO().MouseWheel);
        width *= scale;
        height *= scale;
        window->StateStorage.SetFloat((ImGuiID)StorageValues::WIDTH, width);
        window->StateStorage.SetFloat((ImGuiID)StorageValues::HEIGHT, height);
    }
    // Handle panning
    if (ImGui::IsMouseReleased(1)) {
        window->StateStorage.SetBool((ImGuiID)StorageValues::IS_PANNING, false);
    }
    if (window->StateStorage.GetBool((ImGuiID)StorageValues::IS_PANNING, false)) {
        const ImVec2 drag_offset = ImGui::GetMouseDragDelta(1);
        from_x = start_pan.x;
        from_y = start_pan.y;
        from_x -= drag_offset.x * width / (inner_bb.Max.x - inner_bb.Min.x);
        from_y += drag_offset.y * height / (inner_bb.Max.y - inner_bb.Min.y);
        window->StateStorage.SetFloat((ImGuiID)StorageValues::FROM_X, from_x);
        window->StateStorage.SetFloat((ImGuiID)StorageValues::FROM_Y, from_y);
    } else if (ImGui::IsMouseDragging(1) && ImGui::IsItemHovered()) {
        window->StateStorage.SetBool((ImGuiID)StorageValues::IS_PANNING, true);
        start_pan.x = from_x;
        start_pan.y = from_y;
    }

    // Curves
    int changed_idx = -1;
    for (int point_idx = points_count - 2; point_idx >= 0; --point_idx) {
        ImVec2* points;
        if (flags & (int)CurveEditorFlags::NO_TANGENTS) {
            points = ((ImVec2*)values) + point_idx;
        } else {
            points = ((ImVec2*)values) + 1 + point_idx * 3;
        }

        ImVec2 p_prev = points[0];
        ImVec2 tangent_last;
        ImVec2 tangent;
        ImVec2 p;
        if (flags & (int)CurveEditorFlags::NO_TANGENTS) {
            p = points[1];
        } else {
            tangent_last = points[1];
            tangent = points[2];
            p = points[3];
        }

        ImGui::PushID(point_idx);
        if ((flags & (int)CurveEditorFlags::NO_TANGENTS) == 0) {
            window->DrawList->AddBezierCurve(
                Curve::Transform(p_prev, from_x, from_y, width, height, inner_bb),
                Curve::Transform(p_prev + tangent_last, from_x, from_y, width, height, inner_bb),
                Curve::Transform(p + tangent, from_x, from_y, width, height, inner_bb),
                Curve::Transform(p, from_x, from_y, width, height, inner_bb),
                ImGui::GetColorU32(ImGuiCol_PlotLines),
                1.0f,
                20);
            if (Curve::HandleTangent(tangent_last, p_prev, 0, from_x, from_y, width, height, inner_bb, hovered_idx, point_idx)) {
                points[1] = ImClamp(tangent_last, ImVec2(0, -1), ImVec2(1, 1));
                changed_idx = point_idx;
            }
            if (Curve::HandleTangent(tangent, p, 1, from_x, from_y, width, height, inner_bb, hovered_idx, point_idx)) {
                points[2] = ImClamp(tangent, ImVec2(-1, -1), ImVec2(0, 1));
                changed_idx = point_idx + 1;
            }
            if (Curve::HandlePoint(p, from_x, from_y, width, height, inner_bb, hovered_idx, point_idx, 1)) {
                if (p.x <= p_prev.x) p.x = p_prev.x + 0.001f;
                if (point_idx < points_count - 2 && p.x >= points[6].x) {
                    p.x = points[6].x - 0.001f;
                }
                points[3] = p;
                changed_idx = point_idx + 1;
            }

        } else {
            window->DrawList->AddLine(
                Curve::Transform(p_prev, from_x, from_y, width, height, inner_bb),
                Curve::Transform(p, from_x, from_y, width, height, inner_bb),
                ImGui::GetColorU32(ImGuiCol_PlotLines),
                1.0f
            );
            if (Curve::HandlePoint(p, from_x, from_y, width, height, inner_bb, hovered_idx, point_idx, 1)) {
                if (p.x <= p_prev.x) p.x = p_prev.x + 0.001f;
                if (point_idx < points_count - 2 && p.x >= points[2].x) {
                    p.x = points[2].x - 0.001f;
                }
                points[1] = p;
                changed_idx = point_idx + 1;
            }
        }
        if (point_idx == 0) {
            if (Curve::HandlePoint(p_prev, from_x, from_y, width, height, inner_bb, hovered_idx, point_idx, 0)) {
                if (p.x <= p_prev.x) {
                    p_prev.x = p.x - 0.001f;
                }
                points[0] = p_prev;
                changed_idx = point_idx;
            }
        }
        ImGui::PopID();
    }

    ImGui::SetCursorScreenPos(inner_bb.Min);

    ImGui::InvisibleButton("bg", inner_bb.Max - inner_bb.Min);

    // WTH is this?
    if (ImGui::IsItemActive() && ImGui::IsMouseDoubleClicked(0) && new_count) {
        ImVec2 mp = ImGui::GetMousePos();
        ImVec2 new_p = Curve::InvTransform(mp, from_x, from_y, width, height, inner_bb);
        ImVec2* points = (ImVec2*)values;

        if ((flags & (int)CurveEditorFlags::NO_TANGENTS) == 0) {
            points[points_count * 3 + 0] = ImVec2(-0.2f, 0);
            points[points_count * 3 + 1] = new_p;
            points[points_count * 3 + 2] = ImVec2(0.2f, 0);;
            ++*new_count;

            auto compare = [](const void* a, const void* b) -> int {
                const float fa = (((const ImVec2*)a) + 1)->x;
                const float fb = (((const ImVec2*)b) + 1)->x;
                return fa < fb ? -1 : (fa > fb) ? 1 : 0;
            };

            qsort(values, points_count + 1, sizeof(ImVec2) * 3, compare);

        } else {
            points[points_count] = new_p;
            ++*new_count;

            auto compare = [](const void* a, const void* b) -> int {
                const float fa = ((const ImVec2*)a)->x;
                const float fb = ((const ImVec2*)b)->x;
                return fa < fb ? -1 : (fa > fb) ? 1 : 0;
            };

            qsort(values, points_count + 1, sizeof(ImVec2), compare);
        }
    }

    // No idea what this is for yet...
    if (hovered_idx >= 0 && ImGui::IsMouseDoubleClicked(0) && new_count && points_count > 2) {
        ImVec2* points = (ImVec2*)values;
        --*new_count;
        if ((flags & (int)CurveEditorFlags::NO_TANGENTS) == 0) {
            for (int j = hovered_idx * 3; j < points_count * 3 - 3; j += 3) {
                points[j + 0] = points[j + 3];
                points[j + 1] = points[j + 4];
                points[j + 2] = points[j + 5];
            }
        } else {
            for (int j = hovered_idx; j < points_count - 1; ++j) {
                points[j] = points[j + 1];
            }
        }
    }

    ImGui::EndChildFrame();
//    ImGui::RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, inner_bb.Min.y), label);
    return changed_idx;
}
