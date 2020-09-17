<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile>
  <compound kind="file">
    <name>XmUGrid2dDataExtractor.cpp</name>
    <path>/home/conan/xmsextractor/extractor/</path>
    <filename>_xm_u_grid2d_data_extractor_8cpp</filename>
    <includes id="_xm_u_grid2d_data_extractor_8h" name="XmUGrid2dDataExtractor.h" local="no" imported="no">xmsextractor/extractor/XmUGrid2dDataExtractor.h</includes>
    <includes id="_xm_u_grid_triangles2d_8h" name="XmUGridTriangles2d.h" local="no" imported="no">xmsextractor/ugrid/XmUGridTriangles2d.h</includes>
    <includes id="_xm_u_grid2d_polyline_data_extractor_8h" name="XmUGrid2dPolylineDataExtractor.h" local="no" imported="no">xmsextractor/extractor/XmUGrid2dPolylineDataExtractor.h</includes>
    <includes id="_xm_u_grid2d_data_extractor_8t_8h" name="XmUGrid2dDataExtractor.t.h" local="no" imported="no">xmsextractor/extractor/XmUGrid2dDataExtractor.t.h</includes>
    <class kind="class">xms::XmUGrid2dDataExtractorImpl</class>
    <namespace>xms</namespace>
  </compound>
  <compound kind="file">
    <name>XmUGrid2dDataExtractor.h</name>
    <path>/home/conan/xmsextractor/extractor/</path>
    <filename>_xm_u_grid2d_data_extractor_8h</filename>
    <class kind="class">xms::XmUGrid2dDataExtractor</class>
    <namespace>xms</namespace>
    <member kind="enumeration">
      <type></type>
      <name>DataLocationEnum</name>
      <anchorfile>namespacexms.html</anchorfile>
      <anchor>a7c43691aa00f663af1587d02c5d5260d</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>XmUGrid2dDataExtractor.t.h</name>
    <path>/home/conan/xmsextractor/extractor/</path>
    <filename>_xm_u_grid2d_data_extractor_8t_8h</filename>
    <class kind="class">XmUGrid2dDataExtractorUnitTests</class>
  </compound>
  <compound kind="file">
    <name>XmUGrid2dPolylineDataExtractor.cpp</name>
    <path>/home/conan/xmsextractor/extractor/</path>
    <filename>_xm_u_grid2d_polyline_data_extractor_8cpp</filename>
    <includes id="_xm_u_grid2d_polyline_data_extractor_8h" name="XmUGrid2dPolylineDataExtractor.h" local="no" imported="no">xmsextractor/extractor/XmUGrid2dPolylineDataExtractor.h</includes>
    <includes id="_xm_u_grid2d_data_extractor_8h" name="XmUGrid2dDataExtractor.h" local="no" imported="no">xmsextractor/extractor/XmUGrid2dDataExtractor.h</includes>
    <includes id="_xm_u_grid_triangles2d_8h" name="XmUGridTriangles2d.h" local="no" imported="no">xmsextractor/ugrid/XmUGridTriangles2d.h</includes>
    <includes id="_xm_u_grid2d_polyline_data_extractor_8t_8h" name="XmUGrid2dPolylineDataExtractor.t.h" local="no" imported="no">xmsextractor/extractor/XmUGrid2dPolylineDataExtractor.t.h</includes>
    <namespace>xms</namespace>
    <member kind="variable" protection="private">
      <type>BSHP&lt; XmUGrid2dDataExtractor &gt;</type>
      <name>m_extractor</name>
      <anchorfile>_xm_u_grid2d_polyline_data_extractor_8cpp.html</anchorfile>
      <anchor>a7427151beca06cff3d4f7084e0a2536a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>BSHP&lt; GmMultiPolyIntersector &gt;</type>
      <name>m_multiPolyIntersector</name>
      <anchorfile>_xm_u_grid2d_polyline_data_extractor_8cpp.html</anchorfile>
      <anchor>a8a4eb6ca67d923efb3a8b02bd8c8c0c3</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>XmUGrid2dPolylineDataExtractor.h</name>
    <path>/home/conan/xmsextractor/extractor/</path>
    <filename>_xm_u_grid2d_polyline_data_extractor_8h</filename>
    <includes id="_xm_u_grid2d_data_extractor_8h" name="XmUGrid2dDataExtractor.h" local="no" imported="no">xmsextractor/extractor/XmUGrid2dDataExtractor.h</includes>
    <class kind="class">xms::XmUGrid2dPolylineDataExtractor</class>
    <namespace>xms</namespace>
  </compound>
  <compound kind="file">
    <name>XmUGrid2dPolylineDataExtractor.t.h</name>
    <path>/home/conan/xmsextractor/extractor/</path>
    <filename>_xm_u_grid2d_polyline_data_extractor_8t_8h</filename>
    <class kind="class">XmUGrid2dPolylineDataExtractorUnitTests</class>
  </compound>
  <compound kind="file">
    <name>XmUGridTriangles2d.cpp</name>
    <path>/home/conan/xmsextractor/ugrid/</path>
    <filename>_xm_u_grid_triangles2d_8cpp</filename>
    <includes id="_xm_u_grid_triangles2d_8h" name="XmUGridTriangles2d.h" local="no" imported="no">xmsextractor/ugrid/XmUGridTriangles2d.h</includes>
    <includes id="_xm_u_grid_triangles2d_8t_8h" name="XmUGridTriangles2d.t.h" local="no" imported="no">xmsextractor/ugrid/XmUGridTriangles2d.t.h</includes>
    <namespace>xms</namespace>
    <member kind="variable" protection="private">
      <type>BSHP&lt; VecPt3d &gt;</type>
      <name>m_points</name>
      <anchorfile>_xm_u_grid_triangles2d_8cpp.html</anchorfile>
      <anchor>a344935f61066d6fd29e2525faa0bd845</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>BSHP&lt; VecInt &gt;</type>
      <name>m_triangles</name>
      <anchorfile>_xm_u_grid_triangles2d_8cpp.html</anchorfile>
      <anchor>ad7514828330bfe08bbf399440b0f80d8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>VecInt</type>
      <name>m_centroidIdxs</name>
      <anchorfile>_xm_u_grid_triangles2d_8cpp.html</anchorfile>
      <anchor>a4ad021e9f98f998d036f4c2216368843</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>VecInt</type>
      <name>m_triangleToCellIdx</name>
      <anchorfile>_xm_u_grid_triangles2d_8cpp.html</anchorfile>
      <anchor>a28545e879f609571d86d9366e59f04d1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>BSHP&lt; GmTriSearch &gt;</type>
      <name>m_triSearch</name>
      <anchorfile>_xm_u_grid_triangles2d_8cpp.html</anchorfile>
      <anchor>a55bbeff181425a714c8ebf4244419880</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>XmUGridTriangles2d.h</name>
    <path>/home/conan/xmsextractor/ugrid/</path>
    <filename>_xm_u_grid_triangles2d_8h</filename>
    <class kind="class">xms::XmUGridTriangles2d</class>
    <namespace>xms</namespace>
  </compound>
  <compound kind="file">
    <name>XmUGridTriangles2d.t.h</name>
    <path>/home/conan/xmsextractor/ugrid/</path>
    <filename>_xm_u_grid_triangles2d_8t_8h</filename>
    <class kind="class">XmUGridTriangles2dUnitTests</class>
  </compound>
  <compound kind="class">
    <name>xms::XmUGrid2dDataExtractor</name>
    <filename>classxms_1_1_xm_u_grid2d_data_extractor.html</filename>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~XmUGrid2dDataExtractor</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor.html</anchorfile>
      <anchor>a3f2b216a28513acf78ac9e602f3c9bd8</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual void</type>
      <name>SetGridPointScalars</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor.html</anchorfile>
      <anchor>ac62db70dcc9b182ef8fe9cfc4d08f491</anchor>
      <arglist>(const VecFlt &amp;a_pointScalars, const DynBitset &amp;a_activity, DataLocationEnum a_activityType)=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual void</type>
      <name>SetGridCellScalars</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor.html</anchorfile>
      <anchor>a342f4176c48479be61f6da312b8f56a2</anchor>
      <arglist>(const VecFlt &amp;a_cellScalars, const DynBitset &amp;a_activity, DataLocationEnum a_activityType)=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual void</type>
      <name>SetExtractLocations</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor.html</anchorfile>
      <anchor>aa7a65fb2b6d6892f33ef3d6b1fcf4417</anchor>
      <arglist>(const VecPt3d &amp;a_locations)=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual void</type>
      <name>ExtractData</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor.html</anchorfile>
      <anchor>a2fa7c4efd25bfda5bd8d5a634dae9689</anchor>
      <arglist>(VecFlt &amp;a_outData)=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual float</type>
      <name>ExtractAtLocation</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor.html</anchorfile>
      <anchor>a7212643d2783e9d99b4efeb397f05bb8</anchor>
      <arglist>(const Pt3d &amp;a_location)=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual void</type>
      <name>SetUseIdwForPointData</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor.html</anchorfile>
      <anchor>a08e3e573932038b6a555f6714e904a62</anchor>
      <arglist>(bool a_useIdw)=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual void</type>
      <name>SetNoDataValue</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor.html</anchorfile>
      <anchor>a6f52d0b2fa995083792030922d175002</anchor>
      <arglist>(float a_noDataValue)=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual void</type>
      <name>BuildTriangles</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor.html</anchorfile>
      <anchor>a56b86773f747a14bff08e78a75f97b17</anchor>
      <arglist>(DataLocationEnum a_location)=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual const BSHP&lt; XmUGridTriangles2d &gt;</type>
      <name>GetUGridTriangles</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor.html</anchorfile>
      <anchor>a92c12206ec3d25711144716a669e897c</anchor>
      <arglist>() const =0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual const VecFlt &amp;</type>
      <name>GetScalars</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor.html</anchorfile>
      <anchor>a4091816e6bf7a78585d8e1e24da39cc9</anchor>
      <arglist>() const =0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual DataLocationEnum</type>
      <name>GetScalarLocation</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor.html</anchorfile>
      <anchor>ab99e189167de14f968f3851b9ab23b80</anchor>
      <arglist>() const =0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual const VecPt3d &amp;</type>
      <name>GetExtractLocations</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor.html</anchorfile>
      <anchor>ac3da3ac0adfb6b240b9c984270e4c798</anchor>
      <arglist>() const =0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual const VecInt &amp;</type>
      <name>GetCellIndexes</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor.html</anchorfile>
      <anchor>a2f858c0ab4382221403059e16f2e5944</anchor>
      <arglist>() const =0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual bool</type>
      <name>GetUseIdwForPointData</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor.html</anchorfile>
      <anchor>a863cc47e4ca65953ccfc560a39e2c473</anchor>
      <arglist>() const =0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual float</type>
      <name>GetNoDataValue</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor.html</anchorfile>
      <anchor>a5fd2a9634c40f69d9a6f6f0940bca3f8</anchor>
      <arglist>() const =0</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static BSHP&lt; XmUGrid2dDataExtractor &gt;</type>
      <name>New</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor.html</anchorfile>
      <anchor>a1d5729c24ab03c5d04ad7801acabaf95</anchor>
      <arglist>(std::shared_ptr&lt; XmUGrid &gt; a_ugrid)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static BSHP&lt; XmUGrid2dDataExtractor &gt;</type>
      <name>New</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor.html</anchorfile>
      <anchor>ad685d4c9e255e3ef4dc10dc756346aac</anchor>
      <arglist>(BSHP&lt; XmUGrid2dDataExtractor &gt; a_extractor)</arglist>
    </member>
    <member kind="function" protection="protected">
      <type></type>
      <name>XmUGrid2dDataExtractor</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor.html</anchorfile>
      <anchor>ad7c2eb4dfe420cae920a9cfec7e1f541</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>xms::XmUGrid2dDataExtractorImpl</name>
    <filename>classxms_1_1_xm_u_grid2d_data_extractor_impl.html</filename>
    <base>xms::XmUGrid2dDataExtractor</base>
    <member kind="function">
      <type></type>
      <name>XmUGrid2dDataExtractorImpl</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor_impl.html</anchorfile>
      <anchor>a3453d18b11429e2c55f80e6cc637ff48</anchor>
      <arglist>(std::shared_ptr&lt; XmUGrid &gt; a_ugrid)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>XmUGrid2dDataExtractorImpl</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor_impl.html</anchorfile>
      <anchor>a3b40f4a71bb0666bc8160db574cd3c7f</anchor>
      <arglist>(BSHP&lt; XmUGrid2dDataExtractorImpl &gt; a_extractor)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>SetGridPointScalars</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor_impl.html</anchorfile>
      <anchor>ac07d96067a1b4c1ed1a96f6884a053c7</anchor>
      <arglist>(const VecFlt &amp;a_pointScalars, const DynBitset &amp;a_activity, DataLocationEnum a_activityLocation) override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>SetGridCellScalars</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor_impl.html</anchorfile>
      <anchor>af81d3aef68fc153e11a3099709bfe7bd</anchor>
      <arglist>(const VecFlt &amp;a_cellScalars, const DynBitset &amp;a_activity, DataLocationEnum a_activityLocation) override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>SetExtractLocations</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor_impl.html</anchorfile>
      <anchor>ad127bd45211ee7d0a58d9b666b492550</anchor>
      <arglist>(const VecPt3d &amp;a_locations) override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>ExtractData</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor_impl.html</anchorfile>
      <anchor>abef62849982116cc63b0c35f9f6b007e</anchor>
      <arglist>(VecFlt &amp;a_outData) override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual float</type>
      <name>ExtractAtLocation</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor_impl.html</anchorfile>
      <anchor>aec0a75fa3bda8122b36bf85483dcac19</anchor>
      <arglist>(const Pt3d &amp;a_location) override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>SetUseIdwForPointData</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor_impl.html</anchorfile>
      <anchor>a88d8a8cc7dad124dc22645ec7f3fcb4f</anchor>
      <arglist>(bool a_) override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>SetNoDataValue</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor_impl.html</anchorfile>
      <anchor>a72f9561fd1d2df5f9fdcffa972f06d37</anchor>
      <arglist>(float a_value) override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>BuildTriangles</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor_impl.html</anchorfile>
      <anchor>a526d2f6d716b1daa46f1548ad5de0c7b</anchor>
      <arglist>(DataLocationEnum a_location) override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual const BSHP&lt; XmUGridTriangles2d &gt;</type>
      <name>GetUGridTriangles</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor_impl.html</anchorfile>
      <anchor>a304725c38fa15821b9d62996fa87a370</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual const VecFlt &amp;</type>
      <name>GetScalars</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor_impl.html</anchorfile>
      <anchor>a6f87ec5e4a57c43bb7078aec8d33e8e8</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual DataLocationEnum</type>
      <name>GetScalarLocation</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor_impl.html</anchorfile>
      <anchor>ac27c7ce91f3b6465e0adb9e320a7e6e2</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual const VecPt3d &amp;</type>
      <name>GetExtractLocations</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor_impl.html</anchorfile>
      <anchor>a736196f76954875e201d0c76dd1908fe</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual const VecInt &amp;</type>
      <name>GetCellIndexes</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor_impl.html</anchorfile>
      <anchor>a32c4b6447d001d7b41d9d159934b9ca1</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>GetUseIdwForPointData</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor_impl.html</anchorfile>
      <anchor>a4946847c2bcb04c77fddcff6587b8448</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual float</type>
      <name>GetNoDataValue</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor_impl.html</anchorfile>
      <anchor>ad0ee02f52175263476c46eb951b5bb05</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>ApplyActivity</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor_impl.html</anchorfile>
      <anchor>a2e132d82a291419449a1cd30cb64b468</anchor>
      <arglist>(const DynBitset &amp;a_activity, DataLocationEnum a_location, DynBitset &amp;a_cellActivity)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>SetGridPointActivity</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor_impl.html</anchorfile>
      <anchor>a68c3786154059eeab43d81eae697c9e6</anchor>
      <arglist>(const DynBitset &amp;a_pointActivity, DynBitset &amp;a_cellActivity)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>SetGridCellActivity</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor_impl.html</anchorfile>
      <anchor>aa46eb03b9e43e45e568bbcdfd518a1fa</anchor>
      <arglist>(const DynBitset &amp;a_cellActivity)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>PushPointDataToCentroids</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor_impl.html</anchorfile>
      <anchor>aa4566c77fefafd95535c3999f7be1732</anchor>
      <arglist>(const DynBitset &amp;a_cellActivity)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>PushCellDataToTrianglePoints</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor_impl.html</anchorfile>
      <anchor>acdbda6c138e89a7a12165af1211d0eb9</anchor>
      <arglist>(const VecFlt &amp;a_cellScalars, const DynBitset &amp;a_cellActivity)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>float</type>
      <name>CalculatePointByAverage</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor_impl.html</anchorfile>
      <anchor>a4a8ccbb4e5caa9caf5dd5da14a2e4fe8</anchor>
      <arglist>(const VecInt &amp;a_cellIdxs, const VecFlt &amp;a_cellScalars, const DynBitset &amp;a_cellActivity)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>float</type>
      <name>CalculatePointByIdw</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor_impl.html</anchorfile>
      <anchor>aa03910369227960a727f13476696dd23</anchor>
      <arglist>(int a_pointIdx, const VecInt &amp;a_cellIdxs, const VecFlt &amp;a_cellScalars, const DynBitset &amp;a_cellActivity)</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::shared_ptr&lt; XmUGrid &gt;</type>
      <name>m_ugrid</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor_impl.html</anchorfile>
      <anchor>ac05bcd2dc3774f0696692885b562a49b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>DataLocationEnum</type>
      <name>m_triangleType</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor_impl.html</anchorfile>
      <anchor>ad014c5fa3d603a24ce9438f031b57f52</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>BSHP&lt; XmUGridTriangles2d &gt;</type>
      <name>m_triangles</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor_impl.html</anchorfile>
      <anchor>a4394c7680ed54a1e7274bff5967134c2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>VecPt3d</type>
      <name>m_extractLocations</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor_impl.html</anchorfile>
      <anchor>a4921af052219d59ae589ba817e5f36b0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>VecFlt</type>
      <name>m_pointScalars</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor_impl.html</anchorfile>
      <anchor>a8129ddcb859d261355155d6a6b8b42d1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>VecInt</type>
      <name>m_cellIdxs</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor_impl.html</anchorfile>
      <anchor>a5b044258750bc25c57cbb97add368927</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>bool</type>
      <name>m_useIdwForPointData</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor_impl.html</anchorfile>
      <anchor>a6d07cf6b9b82f04ab6432b37f3e789a5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>float</type>
      <name>m_noDataValue</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_data_extractor_impl.html</anchorfile>
      <anchor>ac468ffa4504499964642fecd2428b645</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>XmUGrid2dDataExtractorUnitTests</name>
    <filename>class_xm_u_grid2d_data_extractor_unit_tests.html</filename>
    <member kind="function">
      <type>void</type>
      <name>testPointScalarsOnly</name>
      <anchorfile>class_xm_u_grid2d_data_extractor_unit_tests.html</anchorfile>
      <anchor>a592c601d3230d9aebf02832f03eafa84</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>testPointScalarCellActivity</name>
      <anchorfile>class_xm_u_grid2d_data_extractor_unit_tests.html</anchorfile>
      <anchor>a8554ab5121d3e6b6de017568fab77e61</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>testPointScalarPointActivity</name>
      <anchorfile>class_xm_u_grid2d_data_extractor_unit_tests.html</anchorfile>
      <anchor>a28fb2f2ed3bca38e45b376ec31a61e39</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>testInvalidPointScalarsAndActivitySize</name>
      <anchorfile>class_xm_u_grid2d_data_extractor_unit_tests.html</anchorfile>
      <anchor>a7b931c82a8c801039a67ae4096e88735</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>testCellScalarsOnly</name>
      <anchorfile>class_xm_u_grid2d_data_extractor_unit_tests.html</anchorfile>
      <anchor>a2e63ce56acec6667345ba0d78cec85f5</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>testCellScalarCellActivity</name>
      <anchorfile>class_xm_u_grid2d_data_extractor_unit_tests.html</anchorfile>
      <anchor>abd5f951d0c1bcd9daafba9f0ee84c4f7</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>testCellScalarCellActivityIdw</name>
      <anchorfile>class_xm_u_grid2d_data_extractor_unit_tests.html</anchorfile>
      <anchor>a5c9f32db6b3a38ad8a5245cb28a589f0</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>testCellScalarPointActivity</name>
      <anchorfile>class_xm_u_grid2d_data_extractor_unit_tests.html</anchorfile>
      <anchor>a5c24a33eb602dca27cf37ede36dcc996</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>testInvalidCellScalarsAndActivitySize</name>
      <anchorfile>class_xm_u_grid2d_data_extractor_unit_tests.html</anchorfile>
      <anchor>a8fb4e5f82936d64cfc66f45dbdcb2196</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>testChangingScalarsAndActivity</name>
      <anchorfile>class_xm_u_grid2d_data_extractor_unit_tests.html</anchorfile>
      <anchor>afeee084b415b6198fcd8646106b6b7a6</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>testCopiedExtractor</name>
      <anchorfile>class_xm_u_grid2d_data_extractor_unit_tests.html</anchorfile>
      <anchor>a11c1970a90110cc4fd22ca63ca202a2c</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>testTutorial</name>
      <anchorfile>class_xm_u_grid2d_data_extractor_unit_tests.html</anchorfile>
      <anchor>af7d9b191fca34dc21625eeac8320b858</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>xms::XmUGrid2dPolylineDataExtractor</name>
    <filename>classxms_1_1_xm_u_grid2d_polyline_data_extractor.html</filename>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~XmUGrid2dPolylineDataExtractor</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_polyline_data_extractor.html</anchorfile>
      <anchor>a1c35a110555d9e417216b2ce96db4b5b</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual BSHP&lt; XmUGrid2dDataExtractor &gt;</type>
      <name>GetDataExtractor</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_polyline_data_extractor.html</anchorfile>
      <anchor>a9afea04e7f04bfcf6c1b4f2ec1b91719</anchor>
      <arglist>() const =0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual void</type>
      <name>SetGridScalars</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_polyline_data_extractor.html</anchorfile>
      <anchor>aaf8f73088f81fc35027e3736250acf98</anchor>
      <arglist>(const VecFlt &amp;a_scalars, const DynBitset &amp;a_activity, DataLocationEnum a_activityLocation)=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual void</type>
      <name>SetPolyline</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_polyline_data_extractor.html</anchorfile>
      <anchor>adaa47cd57ffbb81c7d1f13fba6ceacf9</anchor>
      <arglist>(const VecPt3d &amp;a_polyline)=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual void</type>
      <name>ExtractData</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_polyline_data_extractor.html</anchorfile>
      <anchor>ac436fa359e4a15aa15c2f7dd37302785</anchor>
      <arglist>(VecFlt &amp;a_extractedData)=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual void</type>
      <name>ComputeLocationsAndExtractData</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_polyline_data_extractor.html</anchorfile>
      <anchor>a98ab525141dedc65d8e5c24c2d6677d3</anchor>
      <arglist>(const VecPt3d &amp;a_polyline, VecFlt &amp;a_extractedData, VecPt3d &amp;a_extractedLocations)=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual void</type>
      <name>SetUseIdwForPointData</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_polyline_data_extractor.html</anchorfile>
      <anchor>a7bd2aabd7e52ef44822c0dbd047d512a</anchor>
      <arglist>(bool a_useIdw)=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual void</type>
      <name>SetNoDataValue</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_polyline_data_extractor.html</anchorfile>
      <anchor>ac85228233ff19b8e9a53f27d8319e1eb</anchor>
      <arglist>(float a_noDataValue)=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual const VecFlt &amp;</type>
      <name>GetScalars</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_polyline_data_extractor.html</anchorfile>
      <anchor>a7156867ad49bd430a5eb45078cda0722</anchor>
      <arglist>() const =0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual DataLocationEnum</type>
      <name>GetScalarLocation</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_polyline_data_extractor.html</anchorfile>
      <anchor>a98fc0b9e8d5a473905366849132f2461</anchor>
      <arglist>() const =0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual const VecPt3d &amp;</type>
      <name>GetExtractLocations</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_polyline_data_extractor.html</anchorfile>
      <anchor>a256c988ba68dae464f665c6887558e0b</anchor>
      <arglist>() const =0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual const VecInt &amp;</type>
      <name>GetCellIndexes</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_polyline_data_extractor.html</anchorfile>
      <anchor>a6727daeb05faef7322a287d3f4fc93cc</anchor>
      <arglist>() const =0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual bool</type>
      <name>GetUseIdwForPointData</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_polyline_data_extractor.html</anchorfile>
      <anchor>aa82007dc2bc262ad961f29015cedb46e</anchor>
      <arglist>() const =0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual float</type>
      <name>GetNoDataValue</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_polyline_data_extractor.html</anchorfile>
      <anchor>ad8ccb04b96efd23e9b68a91564c90105</anchor>
      <arglist>() const =0</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static BSHP&lt; XmUGrid2dPolylineDataExtractor &gt;</type>
      <name>New</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_polyline_data_extractor.html</anchorfile>
      <anchor>a89f97d180f2f448691037f0ed62c2b6c</anchor>
      <arglist>(std::shared_ptr&lt; XmUGrid &gt; a_ugrid, DataLocationEnum a_scalarLocation)</arglist>
    </member>
    <member kind="function" protection="protected">
      <type></type>
      <name>XmUGrid2dPolylineDataExtractor</name>
      <anchorfile>classxms_1_1_xm_u_grid2d_polyline_data_extractor.html</anchorfile>
      <anchor>abd13fde3e839243810c144dc88a1cbfd</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>XmUGrid2dPolylineDataExtractorUnitTests</name>
    <filename>class_xm_u_grid2d_polyline_data_extractor_unit_tests.html</filename>
    <member kind="function">
      <type>void</type>
      <name>testOneCellOneSegment</name>
      <anchorfile>class_xm_u_grid2d_polyline_data_extractor_unit_tests.html</anchorfile>
      <anchor>afc94131f5ea8e2b2d0d3ab9478fde507</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>testSegmentAllInCell</name>
      <anchorfile>class_xm_u_grid2d_polyline_data_extractor_unit_tests.html</anchorfile>
      <anchor>ad50bb1972b0d11313628de6462456348</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>testSegmentAlongEdge</name>
      <anchorfile>class_xm_u_grid2d_polyline_data_extractor_unit_tests.html</anchorfile>
      <anchor>a4dfafe8e3f630b530c0d661d04d992df</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>testSegmentAllOutside</name>
      <anchorfile>class_xm_u_grid2d_polyline_data_extractor_unit_tests.html</anchorfile>
      <anchor>a2a4dce7e16a701278e299a1529a0d465</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>testSegmentOutToTouch</name>
      <anchorfile>class_xm_u_grid2d_polyline_data_extractor_unit_tests.html</anchorfile>
      <anchor>a471ee09c7b139c26fad840f0822bea7f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>testSegmentTouchToOut</name>
      <anchorfile>class_xm_u_grid2d_polyline_data_extractor_unit_tests.html</anchorfile>
      <anchor>a1ea31983bdf084cd63b072779bed010d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>testSegmentCrossCellPoint</name>
      <anchorfile>class_xm_u_grid2d_polyline_data_extractor_unit_tests.html</anchorfile>
      <anchor>a90fe20c1e7d01b1d39220815980d724c</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>testSegmentAcrossCellIntoSecond</name>
      <anchorfile>class_xm_u_grid2d_polyline_data_extractor_unit_tests.html</anchorfile>
      <anchor>adb4f77963dee94879d0e416a9cb14078</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>testSegmentAcrossSplitCells</name>
      <anchorfile>class_xm_u_grid2d_polyline_data_extractor_unit_tests.html</anchorfile>
      <anchor>aa734d1b62c16beefdd78863b6b327296</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>testTwoSegmentsAcrossOneCell</name>
      <anchorfile>class_xm_u_grid2d_polyline_data_extractor_unit_tests.html</anchorfile>
      <anchor>ab0e8db06eeb81290735dc8475e39a646</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>testTwoSegmentsAllOutside</name>
      <anchorfile>class_xm_u_grid2d_polyline_data_extractor_unit_tests.html</anchorfile>
      <anchor>a517dcc26e1a3e7a5a73b6a373bb0547d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>testTwoSegmentsFirstExiting</name>
      <anchorfile>class_xm_u_grid2d_polyline_data_extractor_unit_tests.html</anchorfile>
      <anchor>a10fa833249cc9d5b3b8b668953eaf9af</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>testTwoSegmentsJoinInCell</name>
      <anchorfile>class_xm_u_grid2d_polyline_data_extractor_unit_tests.html</anchorfile>
      <anchor>a0965cccee429a712d1ab00bb9f91d217</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>testTwoSegmentsJoinOnBoundary</name>
      <anchorfile>class_xm_u_grid2d_polyline_data_extractor_unit_tests.html</anchorfile>
      <anchor>a8da11751a0ced7744ae7c3cfd769a140</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>testThreeSegmentsCrossOnBoundary</name>
      <anchorfile>class_xm_u_grid2d_polyline_data_extractor_unit_tests.html</anchorfile>
      <anchor>a9d595fb63617dd8c022a11dc1b84b03c</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>testCellScalars</name>
      <anchorfile>class_xm_u_grid2d_polyline_data_extractor_unit_tests.html</anchorfile>
      <anchor>a272a82af8c8c8aee6035d525aa2392ac</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>testTransientTutorial</name>
      <anchorfile>class_xm_u_grid2d_polyline_data_extractor_unit_tests.html</anchorfile>
      <anchor>a24f63cbf667443629095cfeb6a2492cd</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>xms::XmUGridTriangles2d</name>
    <filename>classxms_1_1_xm_u_grid_triangles2d.html</filename>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~XmUGridTriangles2d</name>
      <anchorfile>classxms_1_1_xm_u_grid_triangles2d.html</anchorfile>
      <anchor>ab8d9b2c6d47af177d9c11840cd419d32</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual void</type>
      <name>BuildTriangles</name>
      <anchorfile>classxms_1_1_xm_u_grid_triangles2d.html</anchorfile>
      <anchor>a6a3d7145edf485321167623d66ffeecd</anchor>
      <arglist>(const XmUGrid &amp;a_ugrid, bool a_addTriangleCenters)=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual void</type>
      <name>BuildEarcutTriangles</name>
      <anchorfile>classxms_1_1_xm_u_grid_triangles2d.html</anchorfile>
      <anchor>aca724fa713bcd9f147c5846d059fccc1</anchor>
      <arglist>(const XmUGrid &amp;a_ugrid)=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual void</type>
      <name>SetCellActivity</name>
      <anchorfile>classxms_1_1_xm_u_grid_triangles2d.html</anchorfile>
      <anchor>ac7bb0b10689fe7824e852942e62bb93a</anchor>
      <arglist>(const DynBitset &amp;a_cellActivity)=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual const VecPt3d &amp;</type>
      <name>GetPoints</name>
      <anchorfile>classxms_1_1_xm_u_grid_triangles2d.html</anchorfile>
      <anchor>a4c8d058a6b02553ce2e2573d8ac214a7</anchor>
      <arglist>() const =0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual const VecInt &amp;</type>
      <name>GetTriangles</name>
      <anchorfile>classxms_1_1_xm_u_grid_triangles2d.html</anchorfile>
      <anchor>a614604c7ef0738fe56dd371aa426ce0f</anchor>
      <arglist>() const =0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual BSHP&lt; VecPt3d &gt;</type>
      <name>GetPointsPtr</name>
      <anchorfile>classxms_1_1_xm_u_grid_triangles2d.html</anchorfile>
      <anchor>ab09c568ad01457f2dc28fcd8db36d1c1</anchor>
      <arglist>()=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual BSHP&lt; VecInt &gt;</type>
      <name>GetTrianglesPtr</name>
      <anchorfile>classxms_1_1_xm_u_grid_triangles2d.html</anchorfile>
      <anchor>a71e3b177e827ed2af8640bf57425289a</anchor>
      <arglist>()=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual int</type>
      <name>GetCellCentroid</name>
      <anchorfile>classxms_1_1_xm_u_grid_triangles2d.html</anchorfile>
      <anchor>a6aa6012e517834e125ab0e77da5a535f</anchor>
      <arglist>(int a_cellIdx) const =0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual int</type>
      <name>GetIntersectedCell</name>
      <anchorfile>classxms_1_1_xm_u_grid_triangles2d.html</anchorfile>
      <anchor>a3471a393d3b949bbc6d6f5fc6f123991</anchor>
      <arglist>(const Pt3d &amp;a_point, VecInt &amp;a_idxs, VecDbl &amp;a_weights)=0</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static BSHP&lt; XmUGridTriangles2d &gt;</type>
      <name>New</name>
      <anchorfile>classxms_1_1_xm_u_grid_triangles2d.html</anchorfile>
      <anchor>a0c5ef531dc785d0cc8e5b6f17f0410ec</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="protected">
      <type></type>
      <name>XmUGridTriangles2d</name>
      <anchorfile>classxms_1_1_xm_u_grid_triangles2d.html</anchorfile>
      <anchor>aa8e994d313d37cf934bacf54063305ee</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>XmUGridTriangles2dUnitTests</name>
    <filename>class_xm_u_grid_triangles2d_unit_tests.html</filename>
    <member kind="function">
      <type>void</type>
      <name>testBuildCentroidTrianglesOnTriangle</name>
      <anchorfile>class_xm_u_grid_triangles2d_unit_tests.html</anchorfile>
      <anchor>afe09977679d66a69c435a9b3c4810b09</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>testBuildCentroidTrianglesOnQuad</name>
      <anchorfile>class_xm_u_grid_triangles2d_unit_tests.html</anchorfile>
      <anchor>af4fa88a886b21ad3ab0f69304fdb08ed</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>testBuildCentroidTriangles2dCellTypes</name>
      <anchorfile>class_xm_u_grid_triangles2d_unit_tests.html</anchorfile>
      <anchor>aaeeea39d058316d26bc27a2f6f58b5ea</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>testBuildEarcutTriangles</name>
      <anchorfile>class_xm_u_grid_triangles2d_unit_tests.html</anchorfile>
      <anchor>afe81ac4d22b4e1183b85c735ce0b33c5</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>testBuildCentroidAndEarcutTriangles</name>
      <anchorfile>class_xm_u_grid_triangles2d_unit_tests.html</anchorfile>
      <anchor>abb6708022f3e03584f63cc07a3815169</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>xms</name>
    <filename>namespacexms.html</filename>
    <class kind="class">xms::XmUGrid2dDataExtractor</class>
    <class kind="class">xms::XmUGrid2dDataExtractorImpl</class>
    <class kind="class">xms::XmUGrid2dPolylineDataExtractor</class>
    <class kind="class">xms::XmUGridTriangles2d</class>
    <member kind="enumeration">
      <type></type>
      <name>DataLocationEnum</name>
      <anchorfile>namespacexms.html</anchorfile>
      <anchor>a7c43691aa00f663af1587d02c5d5260d</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>md_Extractor_Tutorial</name>
    <title>Extractor_Tutorial</title>
    <filename>md__extractor__tutorial</filename>
    <docanchor file="md__extractor__tutorial" title="Extractor Tutorial">Extractor_Tutorial</docanchor>
    <docanchor file="md__extractor__tutorial" title="Introduction">Intro_Extractor</docanchor>
    <docanchor file="md__extractor__tutorial" title="Example - Simple Location Extractor">Example_Simple_Extractor</docanchor>
    <docanchor file="md__extractor__tutorial" title="Example - Transient Location Extractor">Example_Transient_Extractor</docanchor>
    <docanchor file="md__extractor__tutorial" title="Example - Transient Polyline Extractor">Example_Polyline_Extractor</docanchor>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>xmsextractor</title>
    <filename>index</filename>
    <docanchor file="index" title="xmsextractor ">xmsextractor</docanchor>
    <docanchor file="index" title="Introduction ">XmsextractorIntroduction</docanchor>
    <docanchor file="index" title="License ">XmsextractorLicense</docanchor>
    <docanchor file="index" title="Features ">XmsextractorFeatures</docanchor>
    <docanchor file="index" title="Testing ">XmsextractorTesting</docanchor>
    <docanchor file="index" title="The Code ">XmsextractorTheCode</docanchor>
    <docanchor file="index" title="Namespaces">XmsextractorNamespaces</docanchor>
    <docanchor file="index" title="Interface pattern">XmsextractorInterfacePattern</docanchor>
    <docanchor file="index" title="&quot;xmsextractor&quot; Name ">xmsextractorName</docanchor>
  </compound>
</tagfile>
